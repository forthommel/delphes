#ifndef OldCalorimeter_h
#define OldCalorimeter_h

/** \class OldCalorimeter
 *
 *  Fills calorimeter towers, performs calorimeter resolution smearing,
 *  preselects towers hit by photons and creates energy flow objects.
 *
 *  $Date$
 *  $Revision$
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "classes/DelphesClasses.h"
#include "classes/DelphesModule.h"

#include <map>
#include <set>
#include <vector>

class DelphesFormula;

class OldCalorimeter : public DelphesModule
{
public:
  OldCalorimeter();
  ~OldCalorimeter();

  void Init();
  void Process();
  void Finish();

private:
  typedef std::map<Long64_t, std::pair<Double_t, Double_t> > TFractionMap; //!
  typedef std::map<Double_t, std::set<Double_t> > TBinMap; //!

  Candidate *fTower;
  Double_t fTowerEta, fTowerPhi, fTowerEdges[4];
  Double_t fTowerECalEnergy, fTowerHCalEnergy;
  Double_t fTowerECalNeutralEnergy, fTowerHCalNeutralEnergy;
  Int_t fTowerPhotonHits, fTowerECalHits, fTowerHCalHits, fTowerAllHits;
  Int_t fTowerECalTrackHits, fTowerHCalTrackHits, fTowerTrackAllHits;

  TFractionMap fFractionMap; //!
  TBinMap fBinMap; //!

  std::vector<Double_t> fEtaBins;
  std::vector<std::vector<Double_t> *> fPhiBins;

  std::vector<Long64_t> fTowerHits;

  std::vector<Double_t> fECalFractions;
  std::vector<Double_t> fHCalFractions;

  DelphesFormula *fECalResolutionFormula; //!
  DelphesFormula *fHCalResolutionFormula; //!

  InputHandle<CandidatesCollection> fParticleInputArray; //!
  InputHandle<CandidatesCollection> fTrackInputArray; //!
  OutputHandle<CandidatesCollection> fTowerOutputArray; //!
  OutputHandle<CandidatesCollection> fPhotonOutputArray; //!
  OutputHandle<CandidatesCollection> fEFlowTrackOutputArray; //!
  OutputHandle<CandidatesCollection> fEFlowTowerOutputArray; //!

  CandidatesCollection fTowerECalArray; //!
  CandidatesCollection fTowerHCalArray; //!
  CandidatesCollection fTowerTrackArray; //!
  CandidatesCollection fTowerECalTrackArray; //!
  CandidatesCollection fTowerHCalTrackArray; //!

  void FinalizeTower();
  Double_t LogNormal(Double_t mean, Double_t sigma);

  ClassDef(OldCalorimeter, 1)
};

#endif
