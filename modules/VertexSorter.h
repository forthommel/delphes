#ifndef VertexSorter_h
#define VertexSorter_h

/** \class VertexSorter
 *
 *
 *  Sorts vertices according to different criteria
 *
 *  \authors A. Hart, M. Selvaggi
 *
 *
*/

#include "classes/DelphesClasses.h"
#include "classes/DelphesModule.h"

#include <string>

class VertexSorter : public DelphesModule
{
public:
  VertexSorter() = default;

  void Init();
  void Process();
  void Finish();

private:
  InputHandle<CandidatesCollection> fInputArray; //!
  InputHandle<CandidatesCollection> fTrackInputArray; //!
  InputHandle<CandidatesCollection> fJetInputArray; //!
  InputHandle<CandidatesCollection> fBeamSpotInputArray; //!
  OutputHandle<CandidatesCollection> fOutputArray; //!

  std::string fMethod;

  ClassDef(VertexSorter, 1)
};

#endif
