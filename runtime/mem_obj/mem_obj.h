/*
 * Copyright (c) 2017, Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include "runtime/api/cl_types.h"
#include "runtime/helpers/base_object.h"
#include "runtime/helpers/completion_stamp.h"
#include "runtime/sharings/sharing.h"
#include "runtime/helpers/properties_helper.h"
#include <atomic>
#include <cstdint>
#include <vector>
#include <array>

namespace OCLRT {
class GraphicsAllocation;
struct KernelInfo;
class MemoryManager;
class Context;
class CommandQueue;
class Device;

template <>
struct OpenCLObjectMapper<_cl_mem> {
    typedef class MemObj DerivedType;
};

class MemObj : public BaseObject<_cl_mem> {
  public:
    const static cl_ulong maskMagic = 0xFFFFFFFFFFFFFF00LL;
    static const cl_ulong objectMagic = 0xAB2212340CACDD00LL;

    MemObj(Context *context,
           cl_mem_object_type memObjectType,
           cl_mem_flags flags,
           size_t size,
           void *memoryStorage,
           void *hostPtr,
           GraphicsAllocation *gfxAllocation,
           bool zeroCopy,
           bool isHostPtrSVM,
           bool isObjectRedescrbied);
    ~MemObj() override;

    cl_int getMemObjectInfo(cl_mem_info paramName,
                            size_t paramValueSize,
                            void *paramValue,
                            size_t *paramValueSizeRet);
    cl_int setDestructorCallback(void(CL_CALLBACK *funcNotify)(cl_mem, void *),
                                 void *userData);

    void *getCpuAddress() const;
    void *getHostPtr() const;
    bool getIsObjectRedescribed() const { return isObjectRedescribed; };
    size_t getSize() const;
    cl_mem_flags getFlags() const;
    void setCompletionStamp(CompletionStamp completionStamp, Device *pDevice, CommandQueue *pCmdQ);
    CompletionStamp getCompletionStamp() const;

    void setMapInfo(void *mappedPtr, size_t *size, size_t *offset);
    void *getBasePtrForMap();

    void *getMappedPtr() const { return mapInfo.ptr; };
    MOCKABLE_VIRTUAL void setAllocatedMapPtr(void *allocatedMapPtr);
    void *getAllocatedMapPtr() const { return allocatedMapPtr; }

    MapInfo::SizeArray getMappedSize() const { return mapInfo.size; }
    MapInfo::OffsetArray getMappedOffset() const { return mapInfo.offset; }

    void setHostPtrMinSize(size_t size);
    void releaseAllocatedMapPtr();

    void incMapCount();
    void decMapCount();
    bool isMemObjZeroCopy() const;
    bool isMemObjWithHostPtrSVM() const;
    virtual void transferDataToHostPtr(std::array<size_t, 3> copySize, std::array<size_t, 3> copyOffset) { UNRECOVERABLE_IF(true); };
    virtual void transferDataFromHostPtr(std::array<size_t, 3> copySize, std::array<size_t, 3> copyOffset) { UNRECOVERABLE_IF(true); };

    GraphicsAllocation *getGraphicsAllocation();
    GraphicsAllocation *getMcsAllocation() { return mcsAllocation; }
    void setMcsAllocation(GraphicsAllocation *alloc) { mcsAllocation = alloc; }

    bool readMemObjFlagsInvalid();
    bool writeMemObjFlagsInvalid();
    bool mapMemObjFlagsInvalid(cl_map_flags mapFlags);

    virtual bool allowTiling() const { return false; }

    CommandQueue *getAssociatedCommandQueue() { return cmdQueuePtr; }
    Device *getAssociatedDevice() { return device; }
    bool isImageFromImage() const { return isImageFromImageCreated; }

    void *getCpuAddressForMapping();
    void *getCpuAddressForMemoryTransfer();

    std::shared_ptr<SharingHandler> &getSharingHandler() { return sharingHandler; }
    SharingHandler *peekSharingHandler() const { return sharingHandler.get(); }
    void setSharingHandler(SharingHandler *sharingHandler) { this->sharingHandler.reset(sharingHandler); }
    void setParentSharingHandler(std::shared_ptr<SharingHandler> &handler) { sharingHandler = handler; }
    unsigned int acquireCount = 0;
    const Context *getContext() const { return context; }

    void waitForCsrCompletion();
    void destroyGraphicsAllocation(GraphicsAllocation *allocation, bool asyncDestroy);
    bool checkIfMemoryTransferIsRequired(size_t offsetInMemObjest, size_t offsetInHostPtr, const void *ptr, cl_command_type cmdType);
    bool mappingOnCpuAllowed() const { return !allowTiling() && !peekSharingHandler(); }

  protected:
    void getOsSpecificMemObjectInfo(const cl_mem_info &paramName, size_t *srcParamSize, void **srcParam);

    void setMappedPtr(void *mappedPtr);
    virtual void setMappedSize(size_t *size) { mapInfo.size[0] = *size; }
    virtual void setMappedOffset(size_t *offset) { mapInfo.offset[0] = *offset; }

    Context *context;
    cl_mem_object_type memObjectType;
    cl_mem_flags flags;
    size_t size;
    size_t hostPtrMinSize = 0;
    void *memoryStorage;
    void *hostPtr;
    void *allocatedMapPtr = nullptr;
    MapInfo mapInfo;
    size_t offset = 0;
    MemObj *associatedMemObject = nullptr;
    std::atomic<uint32_t> mapCount{0};
    cl_uint refCount = 0;
    CompletionStamp completionStamp;
    CommandQueue *cmdQueuePtr = nullptr;
    Device *device = nullptr;
    bool isZeroCopy;
    bool isHostPtrSVM;
    bool isObjectRedescribed;
    bool isImageFromImageCreated = false;
    MemoryManager *memoryManager = nullptr;
    GraphicsAllocation *graphicsAllocation;
    GraphicsAllocation *mcsAllocation = nullptr;
    std::shared_ptr<SharingHandler> sharingHandler;

    class DestructorCallback {
      public:
        DestructorCallback(void(CL_CALLBACK *funcNotify)(cl_mem, void *),
                           void *userData)
            : funcNotify(funcNotify), userData(userData){};

        void invoke(cl_mem memObj);

      private:
        void(CL_CALLBACK *funcNotify)(cl_mem, void *);
        void *userData;
    };

    std::vector<DestructorCallback *> destructorCallbacks;
};
} // namespace OCLRT
