/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'
import { DisabledPanel, PanelWelcome } from 'brave-ui/features/rewards'

// Components
import Panel from './panel'

// Utils
import * as rewardsPanelActions from '../actions/rewards_panel_actions'

interface Props extends RewardsExtension.ComponentProps {
}

interface State {
  windowId: number
}

export class RewardsPanel extends React.Component<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      windowId: -1
    }
  }

  componentDidMount () {
    chrome.windows.getCurrent({}, this.onWindowCallback)
    chrome.braveRewards.getRewardsMainEnabled(((enabled: boolean) => {
      this.props.actions.onEnabledMain(enabled)
    }))
  }

  componentDidUpdate (prevProps: Props, prevState: State) {
    if (
      !prevProps.rewardsPanelData.walletCreated &&
      this.props.rewardsPanelData.walletCreated
    ) {
      this.getTabData()
    }
  }

  getTabData () {
    chrome.tabs.query({
      active: true,
      currentWindow: true
    }, (tabs) => {
      if (!tabs || !tabs.length) {
        return
      }
      this.props.actions.onTabRetrieved(tabs[0])
    })
  }

  getGrant () {
    this.props.actions.getGrant()
  }

  onWindowCallback = (window: chrome.windows.Window) => {
    this.setState({
      windowId: window.id
    })

    if (this.props.rewardsPanelData.walletCreated) {
      this.getGrant()
      this.getTabData()
    }
  }

  openRewards () {
    chrome.tabs.create({
      url: 'chrome://rewards'
    })
  }

  get actions () {
    return this.props.actions
  }

  onCreate = () => {
    this.actions.createWallet()
  }

  render () {
    const {
      enabledMain,
      walletCreateFailed,
      walletCreated,
      walletCreating
    } = this.props.rewardsPanelData

    if (!walletCreated) {
      return (
        <PanelWelcome
          error={walletCreateFailed}
          creating={walletCreating}
          variant={'two'}
          optInAction={this.onCreate}
          optInErrorAction={this.onCreate}
          moreLink={this.openRewards}
        />
      )
    }

    return (
      <>
        {
          enabledMain
          ? <Panel windowId={this.state.windowId} />
          : <div style={{ width: '330px' }}>
              <DisabledPanel onLinkOpen={this.openRewards} />
            </div>
        }
      </>
    )
  }
}

export const mapStateToProps = (state: RewardsExtension.ApplicationState) => ({
  rewardsPanelData: state.rewardsPanelData
})

export const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsPanelActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(RewardsPanel)
