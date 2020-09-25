#if !defined(RootOutputer_h)
#define RootOutputer_h

#include <vector>
#include <string>
#include <cstdint>

#include "OutputerBase.h"
#include "EventIdentifier.h"
#include "DataProductRetriever.h"

#include "SerialTaskQueue.h"

#include "TFile.h"

class TBranch;
class TTree;

namespace cce::tf {
class RootOutputer :public OutputerBase {
 public:
  RootOutputer(std::string const& iFileName, unsigned int iNLanes);
  ~RootOutputer();

  void setupForLane(unsigned int iLaneIndex, std::vector<DataProductRetriever> const& iDPs) final;

  void productReadyAsync(unsigned int iLaneIndex, DataProductRetriever const& iDataProduct, TaskHolder iCallback) const final;
  bool usesProductReadyAsync() const final {return false;}

  void outputAsync(unsigned int iLaneIndex, EventIdentifier const& iEventID, TaskHolder iCallback) const final;
  
  void printSummary() const final;


private:
  void write(unsigned int iLaneIndex);
  TFile file_;
  TTree* eventTree_;
  std::vector<TBranch*> branches_;

  mutable SerialTaskQueue queue_;
  std::vector<std::vector<DataProductRetriever> const*> retrievers_;
  std::chrono::microseconds accumulatedTime_;
};
}
#endif