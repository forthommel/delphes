#include <ROOT/RDataSource.hxx>
#include <TFile.h>

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "modules/Delphes.h"

class RDelphes final : public ROOT::RDF::RDataSource
{
public:
  explicit RDelphes(std::string_view configuration_file, std::string_view output_file) :
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

  ~RDelphes() override
  {
    delphes_->FinishTask();
    tree_writer_->Write();
  }

  const std::vector<std::string> &
  GetColumnNames() const override
  {
    return columns_names_;
  }
  bool HasColumn(std::string_view column_name) const override
  {
    return std::find(columns_names_.begin(), columns_names_.end(), column_name) != columns_names_.end(); //TODO: use std::ranges::find
  }
  std::string GetTypeName(std::string_view column_name) const override { return columns_types_.at(std::string{column_name}); }

  std::vector<std::pair<unsigned long long, unsigned long long> > GetEntryRanges() override
  {
    return {}; //TODO
  }
  bool SetEntry(unsigned int, unsigned long long) override
  {
    return true; //TODO
  }

  ROOT::RDF::RDataSource::Record_t GetColumnReadersImpl(std::string_view, const std::type_info &) override
  {
    return {}; //TODO
  }

private:
  const std::unique_ptr<ExRootConfReader> conf_reader_{};
  const std::unique_ptr<ExRootTreeWriter> tree_writer_{}; //TODO: will eventually disappear to store in data source memory
  const std::unique_ptr<Delphes> delphes_{};

  //ExRootTreeBranch *branch_event_{nullptr}; //TODO: move to internal memory
  //ExRootTreeBranch *branch_weight_{nullptr};
  TObjArray *all_particle_output_array_{nullptr}; //TODO: move to internal memory
  TObjArray *stable_particle_output_array_{nullptr};
  TObjArray *parton_output_array_{nullptr};

  std::vector<std::string> columns_names_{};
  std::unordered_map<std::string, std::string> columns_types_{};
};

int main()
{
  auto delphes = RDelphes{"../cards/delphes_card_LHeC.tcl", "test_rdelphes.root"};

  return 0;
}
