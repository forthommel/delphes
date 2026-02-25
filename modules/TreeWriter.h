/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
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

#ifndef TreeWriter_h
#define TreeWriter_h

/** \class TreeWriter
 *
 *  Fills ROOT tree branches.
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "classes/DelphesClasses.h"
#include "classes/DelphesModule.h"

#include <map>

class TClass;
class TRefArray;

class ExRootTreeBranch;

class TreeWriter : public DelphesModule
{
public:
  TreeWriter() = default;

  void Init();
  void Process();
  void Finish();

private:
  void FillParticles(const Candidate *candidate, TRefArray *array);

  void ProcessParticles(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessVertices(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessTracks(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessTowers(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessParticleFlowCandidates(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessPhotons(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessElectrons(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessMuons(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessCscCluster(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessTauJets(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessJets(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessMissingET(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessScalarHT(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessRho(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessWeight(ExRootTreeBranch *branch, const CandidatesCollection *array);
  void ProcessHectorHit(ExRootTreeBranch *branch, const CandidatesCollection *array);

#if !defined(__CINT__) && !defined(__CLING__)
  typedef void (TreeWriter::*TProcessMethod)(ExRootTreeBranch *, const CandidatesCollection *); //!

  typedef std::map<ExRootTreeBranch *, std::pair<TProcessMethod, const CandidatesCollection *> > TBranchMap; //!

  TBranchMap fBranchMap; //!

  std::map<TClass *, TProcessMethod> fClassMap; //!
#endif

  std::vector<InputHandle<CandidatesCollection> > fInputCollections;

  ClassDef(TreeWriter, 2)
};

#endif
