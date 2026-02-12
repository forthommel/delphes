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

#include "classes/RDelphes.h"
#include <TCanvas.h>

int main(int argc, char *argv[])
{
  if(argc < 3)
    throw std::runtime_error(std::string{"Usage: "} + argv[0] + " [tcl card] [input LHEF file]");
  const auto tcl_card = std::string{argv[1]}, input_lhef = std::string{argv[2]};

  auto delphes_10_events =
    RDelphes::Make(tcl_card, input_lhef, "test_rdelphes.root")
      .Range(0, 1000)
      .Alias("genElectrons", "ParticlePropagator__electrons")
      .Alias("genMuons", "ParticlePropagator__muons")
      .Define("firstElectronPt", "genElectrons.size() > 0 ? genElectrons[0].PT : -1.")
      .Define("firstMuonPt", "genMuons.size() > 0 ? genMuons[0].PT : -1.");

  delphes_10_events
    .Display({"firstElectronPt", "firstMuonPt"}, 100)
    ->Print();

  auto first_muon_pt_hist = delphes_10_events.Histo1D("firstElectronPt");
  TCanvas c;
  first_muon_pt_hist->Draw();
  c.SaveAs("test.png");

  return 0;
}
