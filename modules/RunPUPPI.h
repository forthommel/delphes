#ifndef RunPUPPI_h
#define RunPUPPI_h

#include "classes/DelphesClasses.h"
#include "classes/DelphesModule.h"

#include <vector>

class PuppiContainer;

class RunPUPPI : public DelphesModule
{

public:
  RunPUPPI() = default;

  void Init();
  void Process();
  void Finish();

private:
  PuppiContainer *fPuppi;
  // puppi parameters
  bool fApplyNoLep;
  double fMinPuppiWeight;
  bool fUseExp;

  std::vector<float> fEtaMinBin;
  std::vector<float> fEtaMaxBin;
  std::vector<float> fPtMinBin;
  std::vector<float> fConeSizeBin;
  std::vector<float> fRMSPtMinBin;
  std::vector<float> fRMSScaleFactorBin;
  std::vector<float> fNeutralMinEBin;
  std::vector<float> fNeutralPtSlope;
  std::vector<bool> fApplyCHS;
  std::vector<bool> fUseCharged;
  std::vector<bool> fApplyLowPUCorr;
  std::vector<int> fMetricId;
  std::vector<int> fCombId;

  InputHandle<CandidatesCollection> fTrackInputArray; //!
  InputHandle<CandidatesCollection> fNeutralInputArray; //!
  InputHandle<CandidatesCollection> fPVInputArray; //!
  OutputHandle<CandidatesCollection> fOutputArray; //!
  OutputHandle<CandidatesCollection> fOutputTrackArray; //!
  OutputHandle<CandidatesCollection> fOutputNeutralArray; //!

  ClassDef(RunPUPPI, 1)
};

#endif
