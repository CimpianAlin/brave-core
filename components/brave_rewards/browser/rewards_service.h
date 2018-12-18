/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_REWARDS_REWARDS_SERVICE_
#define BRAVE_BROWSER_BRAVE_REWARDS_REWARDS_SERVICE_

#include <memory>

#include "base/macros.h"
#include "base/observer_list.h"
#include "brave/components/brave_rewards/browser/content_site.h"
#include "brave/components/brave_rewards/browser/balance_report.h"
#include "brave/components/brave_rewards/browser/publisher_banner.h"
#include "build/build_config.h"
#include "components/sessions/core/session_id.h"
#include "components/keyed_service/core/keyed_service.h"
#include "url/gurl.h"

class PrefRegistrySimple;
class Profile;

namespace ledger {
struct PublisherInfo;
class Ledger;
}

namespace content {
class NavigationHandle;
}

namespace brave_rewards {

bool IsMediaLink(const GURL& url,
                 const GURL& first_party_url,
                 const GURL& referrer);

class RewardsNotificationService;
class RewardsServiceObserver;

using GetCurrentContributeListCallback =
    base::Callback<void(std::unique_ptr<ContentSiteList>,
        uint32_t /* next_record */)>;
using GetAllBalanceReportsCallback = base::Callback<void(
    const std::map<std::string, brave_rewards::BalanceReport>&)>;
using GetWalletPassphraseCallback = base::Callback<void(const std::string&)>;
using GetContributionAmountCallback = base::Callback<void(double)>;
using GetAddressesCallback = base::Callback<void(
    const std::map<std::string, std::string>&)>;

class RewardsService : public KeyedService {
 public:
  RewardsService();
  ~RewardsService() override;

  virtual void CreateWallet() = 0;
  virtual void FetchWalletProperties() = 0;
  virtual void GetCurrentContributeList(
      uint32_t start,
      uint32_t limit,
      const GetCurrentContributeListCallback& callback) = 0;
  virtual void FetchGrant(const std::string& lang, const std::string& paymentId) = 0;
  virtual void GetGrantCaptcha() = 0;
  virtual void SolveGrantCaptcha(const std::string& solution) const = 0;
  virtual void GetWalletPassphrase(
      const GetWalletPassphraseCallback& callback) = 0;
  virtual unsigned int GetNumExcludedSites() const = 0;
  virtual void RecoverWallet(const std::string passPhrase) const = 0;
  virtual void ExcludePublisher(const std::string publisherKey) const = 0;
  virtual void RestorePublishers() = 0;
  virtual void OnLoad(SessionID tab_id, const GURL& gurl) = 0;
  virtual void OnUnload(SessionID tab_id) = 0;
  virtual void OnShow(SessionID tab_id) = 0;
  virtual void OnHide(SessionID tab_id) = 0;
  virtual void OnForeground(SessionID tab_id) = 0;
  virtual void OnBackground(SessionID tab_id) = 0;
  virtual void OnMediaStart(SessionID tab_id) = 0;
  virtual void OnMediaStop(SessionID tab_id) = 0;
  virtual void OnXHRLoad(SessionID tab_id,
                         const GURL& url,
                         const GURL& first_party_url,
                         const GURL& referrer) = 0;
  virtual void OnPostData(SessionID tab_id,
                          const GURL& url,
                          const GURL& first_party_url,
                          const GURL& referrer,
                          const std::string& post_data) = 0;

  virtual uint64_t GetReconcileStamp() const = 0;
  virtual void GetAddresses(const GetAddressesCallback& callback) = 0;
  virtual void SetRewardsMainEnabled(bool enabled) const = 0;
  virtual uint64_t GetPublisherMinVisitTime() const = 0;
  virtual void SetPublisherMinVisitTime(uint64_t duration_in_seconds) const = 0;
  virtual unsigned int GetPublisherMinVisits() const = 0;
  virtual void SetPublisherMinVisits(unsigned int visits) const = 0;
  virtual bool GetPublisherAllowNonVerified() const = 0;
  virtual void SetPublisherAllowNonVerified(bool allow) const = 0;
  virtual bool GetPublisherAllowVideos() const = 0;
  virtual void SetPublisherAllowVideos(bool allow) const = 0;
  virtual void SetContributionAmount(double amount) const = 0;
  virtual void SetUserChangedContribution() const = 0;
  virtual bool GetAutoContribute() const = 0;
  virtual void SetAutoContribute(bool enabled) const = 0;
  virtual void SetTimer(uint64_t time_offset, uint32_t& timer_id) = 0;
  virtual void GetAllBalanceReports(
      const GetAllBalanceReportsCallback& callback) = 0;
  virtual void GetCurrentBalanceReport() = 0;
  virtual bool IsWalletCreated() = 0;
  virtual void GetPublisherActivityFromUrl(uint64_t windowId, const std::string& url, const std::string& favicon_url) = 0;
  virtual void GetContributionAmount(
      const GetContributionAmountCallback& callback) = 0;
  virtual void GetPublisherBanner(const std::string& publisher_id) = 0;
  virtual void OnDonate(const std::string& publisher_key, int amount,
      bool recurring, const ledger::PublisherInfo* publisher_info = NULL) = 0;
  virtual void OnDonate(const std::string& publisher_key, int amount,
      bool recurring, std::unique_ptr<brave_rewards::ContentSite> site) = 0;
  virtual void RemoveRecurring(const std::string& publisher_key) = 0;
  virtual void UpdateRecurringDonationsList() = 0;
  virtual void UpdateTipsList() = 0;
  virtual void SetContributionAutoInclude(
    const std::string& publisher_key, bool excluded, uint64_t windowId) = 0;
  virtual RewardsNotificationService* GetNotificationService() const = 0;
  virtual bool CheckImported() = 0;
  virtual void SetLedgerClient(std::unique_ptr<ledger::Ledger> new_ledger) = 0;
  virtual void SetBackupCompleted() = 0;

  void AddObserver(RewardsServiceObserver* observer);
  void RemoveObserver(RewardsServiceObserver* observer);

  static void RegisterProfilePrefs(PrefRegistrySimple* registry);

 protected:
  base::ObserverList<RewardsServiceObserver> observers_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RewardsService);
};

}  // namespace brave_rewards

#endif  // BRAVE_BROWSER_BRAVE_REWARDS_REWARDS_SERVICE_
