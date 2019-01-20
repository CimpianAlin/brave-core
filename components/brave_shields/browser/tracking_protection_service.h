/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_TRACKING_PROTECTION_SERVICE_H_
#define BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_TRACKING_PROTECTION_SERVICE_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "brave/browser/renderer_host/buildflags/buildflags.h" //For STP
#include "brave/components/brave_shields/browser/base_brave_shields_service.h"
#include "brave/components/brave_shields/browser/dat_file_util.h"
#include "content/public/common/resource_type.h"

class CTPParser;
class TrackingProtectionServiceTest;

#if BUILDFLAG(BRAVE_STP_ENABLED)
class HostContentSettingsMap;
#endif

namespace brave_shields {

const std::string kTrackingProtectionComponentName("Brave Tracking Protection Updater");
const std::string kTrackingProtectionComponentId("afalakplffnnnlkncjhbmahjfjhmlkal");

const std::string kTrackingProtectionComponentBase64PublicKey =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs4TIQXRCftLpGmQZxmm6"
    "AU8pqGKLoDyi537HGQyRKcK7j/CSXCf3vwJr7xkV72p7bayutuzyNZ3740QxBPie"
    "sfBOp8bBb8d2VgTHP3b+SuNmK/rsSRsMRhT05x8AAr/7ab6U3rW0Gsalm2653xnn"
    "QS8vt0s62xQTmC+UMXowaSLUZ0Be/TOu6lHZhOeo0NBMKc6PkOu0R1EEfP7dJR6S"
    "M/v4dBUBZ1HXcuziVbCXVyU51opZCMjlxyUlQR9pTGk+Zh5sDn1Vw1MwLnWiEfQ4"
    "EGL1V7GeI4vgLoOLgq7tmhEratHGCfC1IHm9luMACRr/ybMI6DQJOvgBvecb292F"
    "xQIDAQAB";

// The brave shields service in charge of tracking protection and init.
class TrackingProtectionService : public BaseBraveShieldsService {
 public:
  TrackingProtectionService();
  ~TrackingProtectionService() override;

  bool ShouldStartRequest(const GURL& spec,
    content::ResourceType resource_type,
    const std::string& tab_host) override;
  scoped_refptr<base::SequencedTaskRunner> GetTaskRunner() override;

#if BUILDFLAG(BRAVE_STP_ENABLED)
  bool ShouldStoreState(HostContentSettingsMap* map, 
    int render_process_id, int render_frame_id, const GURL& top_origin_url, 
    const GURL& origin_url);
  void DispatchBlockedEvent(int render_process_id, int render_frame_id, 
    const GURL& request_url);

  void SetStartingSiteForRenderFrame(GURL starting_site, int render_process_id,
    int render_frame_id);
  GURL GetStartingSiteForRenderFrame(int render_process_id,
    int render_frame_id);
  void DeleteRenderFrameKey(int render_process_id, int render_frame_id);
  void ModifyRenderFrameKey(int old_render_process_id, int old_render_frame_id,
    int new_render_process_id, int new_render_frame_id);
#endif
  
 protected:
  bool Init() override;
  void Cleanup() override;
  void OnComponentReady(const std::string& component_id,
      const base::FilePath& install_dir,
      const std::string& manifest) override;

#if BUILDFLAG(BRAVE_STP_ENABLED)
  void ParseStorageTrackersData();
  struct RenderFrameIdKey {
    RenderFrameIdKey();
    RenderFrameIdKey(int render_process_id, int frame_routing_id);

    int render_process_id;
    int frame_routing_id;

    bool operator<(const RenderFrameIdKey& other) const;
    bool operator==(const RenderFrameIdKey& other) const;
  };
#endif

 private:
  friend class ::TrackingProtectionServiceTest;
  static std::string g_tracking_protection_component_id_;
  static std::string g_tracking_protection_component_base64_public_key_;
  static void SetComponentIdAndBase64PublicKeyForTest(
      const std::string& component_id,
      const std::string& component_base64_public_key);

  void OnDATFileDataReady();
  std::vector<std::string> GetThirdPartyHosts(const std::string& base_host);

#if BUILDFLAG(BRAVE_STP_ENABLED)
  std::vector<std::string> first_party_storage_trackers_;
  std::map<RenderFrameIdKey, GURL> render_frame_key_to_starting_site_url;
  base::Lock frame_starting_site_map_lock_;

  brave_shields::DATFileDataBuffer storage_trackers_buffer_;
#endif
  brave_shields::DATFileDataBuffer buffer_;

  std::unique_ptr<CTPParser> tracking_protection_client_;
  // TODO: Temporary hack which matches both browser-laptop and Android code
  std::vector<std::string> white_list_;
  std::vector<std::string> third_party_base_hosts_;
  std::map<std::string, std::vector<std::string>> third_party_hosts_cache_;
  std::mutex third_party_hosts_mutex_;

  SEQUENCE_CHECKER(sequence_checker_);
  base::WeakPtrFactory<TrackingProtectionService> weak_factory_;
  DISALLOW_COPY_AND_ASSIGN(TrackingProtectionService);
};

// Creates the TrackingProtectionService
std::unique_ptr<TrackingProtectionService> TrackingProtectionServiceFactory();

}  // namespace brave_shields

#endif  // BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_TRACKING_PROTECTION_SERVICE_H_
