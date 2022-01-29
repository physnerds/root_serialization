#if !defined(RootEventOutputer_h)
#define RootEventOutputer_h

#include <vector>
#include <string>
#include <cstdint>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "OutputerBase.h"
#include "EventIdentifier.h"
#include "SerializeStrategy.h"
#include "DataProductRetriever.h"
#include "pds_writer.h"

#include "SerialTaskQueue.h"

namespace cce::tf {
class RootEventOutputer :public OutputerBase {
 public:
  RootEventOutputer(std::string const& iFileName, unsigned int iNLanes, pds::Compression iCompression, int iCompressionLevel, 
                    pds::Serialization iSerialization, int autoFlush, int maxVirtualSize );
 ~RootEventOutputer();

  void setupForLane(unsigned int iLaneIndex, std::vector<DataProductRetriever> const& iDPs) final;

  void productReadyAsync(unsigned int iLaneIndex, DataProductRetriever const& iDataProduct, TaskHolder iCallback) const final;
  bool usesProductReadyAsync() const final {return true;}

  void outputAsync(unsigned int iLaneIndex, EventIdentifier const& iEventID, TaskHolder iCallback) const final;
  
  void printSummary() const final;

 private:
  static inline size_t bytesToWords(size_t nBytes) {
    return nBytes/4 + ( (nBytes % 4) == 0 ? 0 : 1);
  }

  void output(EventIdentifier const& iEventID, SerializeStrategy const& iSerializers, std::vector<uint32_t> const& iBuffer);
  void writeMetaData(SerializeStrategy const& iSerializers);

  std::vector<uint32_t> writeDataProductsToOutputBuffer(SerializeStrategy const& iSerializers) const;

  std::pair<std::vector<uint32_t>, int> compressBuffer(std::vector<uint32_t> const& iBuffer) const;

private:
  mutable TFile file_;

  TTree* eventsTree_;

  mutable SerialTaskQueue queue_;
  mutable std::vector<SerializeStrategy> serializers_;
  mutable std::vector<uint32_t> eventBlob_;
  EventIdentifier eventID_;
  pds::Compression compression_;
  int compressionLevel_;
  pds::Serialization serialization_;
  bool firstTime_ = true;
  mutable std::chrono::microseconds serialTime_;
  mutable std::atomic<std::chrono::microseconds::rep> parallelTime_;
};
}
#endif