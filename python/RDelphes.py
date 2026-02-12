#  Delphes: a framework for fast simulation of a generic collider experiment
#  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
#                2026       AGH University of Krakow, Poland
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import ROOT
import argparse


ROOT.gSystem.Load("libDelphes")

parser = argparse.ArgumentParser()
parser.add_argument('tcl-card', help="Path to the TCL card to use", type=str)
parser.add_argument('input-lhef', help="Path to the LHEF input file to use", type=str)
args = parser.parse_args()

delphes_10_events = ROOT.RDelphes.Make(args.tcl_card, args.input_lhef, "test_rdelphes.root") \
    .Range(0, 1000) \
    .Alias("genElectrons", "ParticlePropagator__electrons") \
    .Alias("genMuons", "ParticlePropagator__muons") \
    .Alias("recoMuons", "UniqueObjectFinder__muons") \
    .Define("firstElectronPt", "genElectrons.size() > 0 ? genElectrons[0].PT : -1.") \
    .Define("firstMuonPt", "genMuons.size() > 0 ? genMuons[0].PT : -1.") \
    .Define("firstRecoMuonPt", "recoMuons.size() > 0 ? recoMuons[0].PT : -1.")

delphes_10_events \
    .Display({"firstElectronPt", "firstMuonPt", "firstRecoMuonPt"}, 100).Print()

first_muon_pt_hist = delphes_10_events.Histo1D("firstElectronPt")
c = ROOT.TCanvas()
first_muon_pt_hist.Draw()
c.SaveAs("test.png")

