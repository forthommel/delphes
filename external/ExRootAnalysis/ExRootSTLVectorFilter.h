#ifndef ExRootFilter_h
#define ExRootFilter_h

#include <unordered_map>
#include <vector>

class Candidate;
class ExRootClassifier;

class ExRootSTLVectorFilter
{
public:
  explicit ExRootSTLVectorFilter(const std::vector<Candidate> &collection);

  void Reset(ExRootClassifier *classifier = 0);

  std::vector<Candidate> GetSubArray(ExRootClassifier *classifier, int category);

private:
  const std::vector<Candidate> &fCollection; ///< Reference to the candidates collection

  using TCategoryMap = std::unordered_map<int, std::vector<Candidate> >;
  using TClassifierMap = std::unordered_map<ExRootClassifier *, std::pair<bool, TCategoryMap> >;

  TClassifierMap fMap; //!
};

#endif /* ExRootFilter */
