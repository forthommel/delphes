/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
 *                2026       AGH University of Krakow, Poland
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <TFile.h>
#include <TFolder.h>

#include <algorithm>
#include <iostream>

#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "classes/DelphesClasses.h" //FIXME
#include "classes/DelphesLHEFReader.h"
#include "classes/RDelphes.h"
#include "modules/Delphes.h"

RDelphes::RDelphes(std::string_view configuration_file, std::string_view input_filename, std::string_view output_file) :
  conf_reader_(std::make_unique<ExRootConfReader>()),
  tree_writer_(std::make_unique<ExRootTreeWriter>(TFile::Open(output_file.data(), "recreate"), "Delphes")),
  delphes_(std::make_unique<Delphes>()),
  file_reader_(std::make_unique<DelphesLHEFReader>())
{
  conf_reader_->ReadFile(configuration_file.data());
  delphes_->SetConfReader(conf_reader_.get());
  delphes_->SetTreeWriter(tree_writer_.get());

  all_particle_output_array_ = delphes_->ExportArray("allParticles");
  stable_particle_output_array_ = delphes_->ExportArray("stableParticles");
  parton_output_array_ = delphes_->ExportArray("partons");

  delphes_->InitTask();

  //FIXME: very hacky way of browsing the output content of the export folder,
  // introducing a very minimal (but risky...) API change to the DelphesModule
  // object, allow to retrieve programmatically its export folder.
  for(auto *subfolder : TRangeDynCast<TFolder>(*delphes_->ExportFolder()->GetListOfFolders()))
  {
    if(subfolder && strcmp(subfolder->GetName(), "Export") == 0)
    {
      for(auto *export_subfolder : TRangeDynCast<TFolder>(*subfolder->GetListOfFolders()))
        if(export_subfolder)
        {
          //TODO: retrieve this from the modules built from configuration reader?
          // e.g. for(const auto &[module_name, module_type] : *conf_reader_->GetModules())
          const auto submodule_name = std::string{export_subfolder->GetName()};
          for(auto *collection : TRangeDynCast<TObjArray>(export_subfolder->GetListOfFolders()))
            if(collection)
            {
              const auto collection_name = submodule_name + "__" + std::string{collection->GetName()};
              columns_names_.emplace_back(collection_name);
              columns_collection_addresses_[collection_name] = collection;
              columns_first_object_addresses_[collection_name] = collection->GetObjectRef();
              //std::cout << collection_name << " => " << columns_collection_addresses_.at(collection_name) << " -> " << columns_first_object_addresses_.at(collection_name) << std::endl;
              columns_types_[collection_name] = "ROOT::VecOps::RVec<Candidate>"; //FIXME: dirty byte-by-byte cast...
              //columns_types_[collection_name] = "std::vector<Candidate>"; //FIXME: dirty byte-by-byte cast...
              //columns_types_[collection_name] = "Candidate*"; //FIXME: dirty byte-by-byte cast...
            }
        }
    }
  }

  //TODO: use polymorphism for any 'DelphesReader' derivative
  if(input_lhe_file_ = fopen(input_filename.data(), "r"); input_lhe_file_ != nullptr)
  {
    std::cout << "Opened input events file '" << input_filename << "' for reading." << std::endl;
    file_reader_->SetInputFile(input_lhe_file_);
  }
  else
    throw std::runtime_error(std::string{"Failed to open input events file '"} + std::string{input_filename} + "' for reading.");

  // initialise everything first
  tree_writer_->Clear();
  delphes_->Clear();
  file_reader_->Clear();
}

RDelphes::~RDelphes()
{
  if(input_lhe_file_)
    fclose(input_lhe_file_);
  delphes_->FinishTask();
  tree_writer_->Write();
}

ROOT::RDataFrame RDelphes::Make(std::string_view configuration_file, std::string_view input_file, std::string_view output_file)
{
  auto data_source = std::make_unique<RDelphes>(configuration_file, input_file, output_file);
  return ROOT::RDataFrame(std::move(data_source));
}

const std::vector<std::string> &RDelphes::GetColumnNames() const
{
  std::cout << "Retrieving all columns names." << std::endl;
  return columns_names_;
}

bool RDelphes::HasColumn(std::string_view column_name) const
{
  std::cout << "Checking if column '" << column_name << "' exists." << std::endl;
  return std::find(columns_names_.begin(), columns_names_.end(), column_name) != columns_names_.end(); //TODO: use std::ranges::find
}

std::string RDelphes::GetTypeName(std::string_view column_name) const
{
  if(const auto column_name_str = std::string{column_name}; columns_types_.count(column_name_str) > 0)
  {
    const auto column_type = columns_types_.at(column_name_str);
    std::cout << "Retrieved type '" << column_type << "' for column with name '" << column_name << "'." << std::endl;
    TClass::GetClass(column_type.data()); // make sure it is already loaded in the RTE
    return column_type;
  }
  else
    throw std::runtime_error("Failed to retrieve type for column with name '" + column_name_str + "'. Was it properly initialised by the configuration reader?");
}

std::vector<RDelphes::EntryRange> RDelphes::GetEntryRanges()
{
  return std::vector<EntryRange>{{0, std::numeric_limits<unsigned long long>::max()}}; //TODO
}

bool RDelphes::SetEntry(unsigned int /*slot*/, unsigned long long entry)
{
  if(current_entry_ != entry)
  {
    tree_writer_->Clear();
    delphes_->Clear();
    file_reader_->Clear();
    std::cout << "Reading a new entry #" << entry << "." << std::endl;
    while(file_reader_->ReadBlock(delphes_->GetFactory(), all_particle_output_array_, stable_particle_output_array_, parton_output_array_))
    {
      if(file_reader_->EventReady())
      {
        delphes_->ProcessTask();
        tree_writer_->Fill();
        current_entry_ = entry;
        return true;
      }
    }
  }
  return false;
}

ROOT::RDF::RDataSource::Record_t RDelphes::GetColumnReadersImpl(std::string_view column_name, const std::type_info &)
{
  if(const auto column_name_str = std::string{column_name}; columns_first_object_addresses_.count(column_name_str) > 0)
    return {(void *)&columns_first_object_addresses_.at(column_name_str)};
  else
    throw std::runtime_error("Failed to retrieve a column reader for column with name '" + column_name_str + "'.");
}
