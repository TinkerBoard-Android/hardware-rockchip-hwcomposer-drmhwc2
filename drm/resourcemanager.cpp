/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "hwc-resource-manager"

#include "resourcemanager.h"
#include <cutils/properties.h>
#include <log/log.h>
#include <sstream>
#include <string>


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


namespace android {

ResourceManager::ResourceManager() :
  num_displays_(0) {
  drmGralloc_ = DrmGralloc::getInstance();
}

int ResourceManager::Init() {
  char path_pattern[PROPERTY_VALUE_MAX];
  // Could be a valid path or it can have at the end of it the wildcard %
  // which means that it will try open all devices until an error is met.
  int path_len = property_get("vendor.hwc.drm.device", path_pattern, "/dev/dri/card0");
  int ret = 0;
  if (path_pattern[path_len - 1] != '%') {
    ret = AddDrmDevice(std::string(path_pattern));
  } else {
    path_pattern[path_len - 1] = '\0';
    for (int idx = 0; !ret; ++idx) {
      std::ostringstream path;
      path << path_pattern << idx;
      ret = AddDrmDevice(path.str());
    }
  }

  if (!num_displays_) {
    ALOGE("Failed to initialize any displays");
    return ret ? -EINVAL : ret;
  }

  fb0_fd = open("/dev/graphics/fb0", O_RDWR, 0);
  if(fb0_fd < 0){
    ALOGE("Open fb0 fail in %s",__FUNCTION__);
  }


  return 0;
}

int ResourceManager::AddDrmDevice(std::string path) {
  std::unique_ptr<DrmDevice> drm = std::make_unique<DrmDevice>();
  int displays_added, ret;
  std::tie(ret, displays_added) = drm->Init(path.c_str(), num_displays_);
  if (ret)
    return ret;
  std::shared_ptr<Importer> importer;
  importer.reset(Importer::CreateInstance(drm.get()));
  if (!importer) {
    ALOGE("Failed to create importer instance");
    return -ENODEV;
  }
  importers_.push_back(importer);
  drms_.push_back(std::move(drm));
  num_displays_ += displays_added;
  return ret;
}

DrmConnector *ResourceManager::AvailableWritebackConnector(int display) {
  DrmDevice *drm_device = GetDrmDevice(display);
  DrmConnector *writeback_conn = NULL;
  if (drm_device) {
    writeback_conn = drm_device->AvailableWritebackConnector(display);
    if (writeback_conn)
      return writeback_conn;
  }
  for (auto &drm : drms_) {
    if (drm.get() == drm_device)
      continue;
    writeback_conn = drm->AvailableWritebackConnector(display);
    if (writeback_conn)
      return writeback_conn;
  }
  return writeback_conn;
}

DrmDevice *ResourceManager::GetDrmDevice(int display) {
  for (auto &drm : drms_) {
    if (drm->HandlesDisplay(display))
      return drm.get();
  }
  return NULL;
}

std::shared_ptr<Importer> ResourceManager::GetImporter(int display) {
  for (unsigned int i = 0; i < drms_.size(); i++) {
    if (drms_[i]->HandlesDisplay(display))
      return importers_[i];
  }
  return NULL;
}
struct assign_plane_group{
	int conenctor_type;
  uint64_t drm_type;
};
struct assign_plane_group assign_plane_group_1[] = {
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_CLUSTER0_WIN0 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_CLUSTER0_WIN1 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_CLUSTER1_WIN0 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_CLUSTER1_WIN1 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_ESMART0_WIN0 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_ESMART1_WIN0 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_SMART0_WIN0 },
  { DRM_MODE_CONNECTOR_eDP , DRM_PLANE_TYPE_SMART1_WIN0 },
};

struct assign_plane_group assign_plane_group_2[] = {
  { DRM_MODE_CONNECTOR_eDP   , DRM_PLANE_TYPE_CLUSTER0_WIN0 },
  { DRM_MODE_CONNECTOR_eDP   , DRM_PLANE_TYPE_CLUSTER0_WIN1 },
  { DRM_MODE_CONNECTOR_HDMIA , DRM_PLANE_TYPE_CLUSTER1_WIN0 },
  { DRM_MODE_CONNECTOR_HDMIA , DRM_PLANE_TYPE_CLUSTER1_WIN1 },
  { DRM_MODE_CONNECTOR_eDP   , DRM_PLANE_TYPE_ESMART0_WIN0 },
  { DRM_MODE_CONNECTOR_HDMIA , DRM_PLANE_TYPE_ESMART1_WIN0 },
  { DRM_MODE_CONNECTOR_eDP   , DRM_PLANE_TYPE_SMART0_WIN0 },
  { DRM_MODE_CONNECTOR_HDMIA , DRM_PLANE_TYPE_SMART1_WIN0 },
};

int ResourceManager::assignPlaneGroup(int display){

  DrmDevice * drm = GetDrmDevice(display);
  std::vector<PlaneGroup*> all_plane_group = drm->GetPlaneGroups();
  uint32_t active_display_num = getActiveDisplayCnt();

  if(active_display_num==0){
    ALOGI_IF(LogLevel(DBG_INFO),"%s,line=%d, active_display_num = %u not to assignPlaneGroup",
                                 __FUNCTION__,__LINE__,active_display_num);
    return -1;
  }

  ALOGI_IF(LogLevel(DBG_INFO),"%s,line=%d, active_display_num = %u, display=%d",
                               __FUNCTION__,__LINE__,active_display_num,display);


#if VOP2
  DrmCrtc *crtc = drm->GetCrtcForDisplay(display);
  DrmConnector *connector = drm->GetConnectorForDisplay(display);
  int connector_type = connector->type();
  uint32_t crtc_mask = 1 << crtc->pipe();

  if(active_display_num == 1){
    for(auto &plane_group : all_plane_group){
        plane_group->set_current_crtc(crtc_mask);
    }
  }else if(active_display_num == 2){
    for(auto &plane_group : all_plane_group){
      for(uint32_t i = 0; i < ARRAY_SIZE(assign_plane_group_2); i++){
        int assign_conn_type = assign_plane_group_2[i].conenctor_type;
        uint64_t assign_win_type =  assign_plane_group_2[i].drm_type;
        uint64_t plane_group_win_type = plane_group->planes[0]->win_type();
        if(connector_type == assign_conn_type && plane_group_win_type == assign_win_type){
          plane_group->set_current_crtc(crtc_mask);
        }
      }
    }
  }else{
    for(auto &plane_group : all_plane_group){
        plane_group->set_current_crtc(crtc_mask);
    }
  }

#else
  DrmCrtc *crtc = drm->GetCrtcForDisplay(display);
  uint32_t crtc_mask = 1 << crtc->pipe();
  if(active_display_num == 1){
    for(auto &plane_group : all_plane_group){
        plane_group->set_current_crtc(crtc_mask);
    }
  }else if(active_display_num == 2){
    for(auto &plane_group : all_plane_group){
        plane_group->set_current_crtc(crtc_mask);
    }
  }else{
    for(auto &plane_group : all_plane_group){
        plane_group->set_current_crtc(crtc_mask);
    }
  }
#endif
  return 0;
}


}  // namespace android
