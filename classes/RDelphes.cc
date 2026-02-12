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

#include <algorithm>
#include <iostream>

#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "classes/RDelphes.h"
#include "modules/Delphes.h"

RDelphes::RDelphes(std::string_view configuration_file, std::string_view output_file) :
  conf_reader_(std::make_unique<ExRootConfReader>()),
  tree_writer_(std::make_unique<ExRootTreeWriter>(TFile::Open(output_file.data(), "recreate"), "Delphes")),
  delphes_(std::make_unique<Delphes>())
{
  //branch_event_ = tree_writer_->NewBranch("Event", HepMCEvent::Class());
  //branch_weight_ = tree_writer_->NewBranch("Weight", Weight::Class());

  conf_reader_->ReadFile(configuration_file.data());
  delphes_->SetConfReader(conf_reader_.get());
  delphes_->SetTreeWriter(tree_writer_.get());

  for(const auto &[module_name, module_type] : *conf_reader_->GetModules())
  {
    columns_names_.emplace_back(module_name);
    std::cout << "module: " << module_type << "[" << module_name << "]." << std::endl;
  }

  all_particle_output_array_ = delphes_->ExportArray("allParticles");
  stable_particle_output_array_ = delphes_->ExportArray("stableParticles");
  parton_output_array_ = delphes_->ExportArray("partons");

  delphes_->InitTask();
}

RDelphes::~RDelphes()
{
  delphes_->FinishTask();
  tree_writer_->Write();
}

const std::vector<std::string> &RDelphes::GetColumnNames() const
{
  return columns_names_;
}

bool RDelphes::HasColumn(std::string_view column_name) const
{
  return std::find(columns_names_.begin(), columns_names_.end(), column_name) != columns_names_.end(); //TODO: use std::ranges::find
}

std::string RDelphes::GetTypeName(std::string_view column_name) const
{
  if(const auto column_name_str = std::string{column_name}; columns_types_.count(column_name_str) > 0)
    return columns_types_.at(std::string{column_name});
  else
    throw std::runtime_error("Failed to retrieve type for column with name '" + column_name_str + "'. Was it properly initialised by the configuration reader?");
}

std::vector<std::pair<unsigned long long, unsigned long long> > RDelphes::GetEntryRanges()
{
  return {}; //TODO
}

bool RDelphes::SetEntry(unsigned int /*slot*/, unsigned long long entry)
{
  if(current_entry_ != entry)
  {
    delphes_->ProcessTask();
    current_entry_ = entry;
  }
  return true; //TODO
}

ROOT::RDF::RDataSource::Record_t RDelphes::GetColumnReadersImpl(std::string_view, const std::type_info &)
{
  return {}; //TODO
}
