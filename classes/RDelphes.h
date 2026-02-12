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

#ifndef RDelphes_h
#define RDelphes_h

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>

#include <unordered_map>

class Delphes;
class DelphesLHEFReader;
class ExRootConfReader;
class ExRootTreeWriter;

class RDelphes final : public ROOT::RDF::RDataSource
{
public:
  explicit RDelphes(std::string_view configuration_file, std::string_view input_file, std::string_view output_file);
  ~RDelphes() override;

  static ROOT::RDataFrame Make(std::string_view, std::string_view, std::string_view);

  const std::vector<std::string> &GetColumnNames() const override;
  bool HasColumn(std::string_view column_name) const override;
  std::string GetTypeName(std::string_view column_name) const override;

  using EntryRange = std::pair<unsigned long long, unsigned long long>;
  std::vector<EntryRange> GetEntryRanges() override;
  bool SetEntry(unsigned int /*slot*/, unsigned long long entry) override;

  ROOT::RDF::RDataSource::Record_t GetColumnReadersImpl(std::string_view, const std::type_info &) override;

private:
  const std::unique_ptr<ExRootConfReader> conf_reader_{};
  const std::unique_ptr<ExRootTreeWriter> tree_writer_{}; //TODO: will eventually disappear to store in data source memory
  const std::unique_ptr<Delphes> delphes_{};
  const std::unique_ptr<DelphesLHEFReader> file_reader_{}; //TODO: use polymorphism

  FILE *input_lhe_file_{nullptr};

  TObjArray *all_particle_output_array_{nullptr}; //TODO: move to internal memory
  TObjArray *stable_particle_output_array_{nullptr};
  TObjArray *parton_output_array_{nullptr};

  unsigned long long current_entry_{999ull}; //FIXME

  std::vector<std::string> columns_names_{};
  std::unordered_map<std::string, void *> columns_collection_addresses_{};
  std::unordered_map<std::string, void *> columns_first_object_addresses_{};
  std::unordered_map<std::string, std::string> columns_types_{};
};

#endif
