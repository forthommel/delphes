//------------------------------------------------------------------------------

#ifndef BeamSpotFilter_h
#define BeamSpotFilter_h

/** \class BeamSpotFilter
 *
 *  Extracts beam spot
 *
 *  \author Michele Selvaggi
 *
 */

#include "classes/DelphesClasses.h"
#include "classes/DelphesModule.h"

class BeamSpotFilter : public DelphesModule
{
public:
  BeamSpotFilter() = default;

  void Init();
  void Process();
  void Finish();

private:
  Float_t fPassedOne;

  InputHandle<CandidatesCollection> fInputArray; //!
  OutputHandle<CandidatesCollection> fOutputArray; //!

  ClassDef(BeamSpotFilter, 1)
};

#endif
