tabChangedHandler = (tab) ->
  if tab.url.indexOf(chrome.extension.getURL("index.html")) != -1
    if root._twunerTab
      if tab.id != root._twunerTab.id
        # Duplicate, ues the first one
        showTwunerTab()
    else
      root._twunerTab = tab
  return


sharePage = (info, tab) ->
  shareWithTwuner(tab.title + ' ' + info.pageUrl)
  return

shareSelection = (info, tab) ->
  shareWithTwuner("\"#{info.selectionText}\" via: #{info.pageUrl}")
  return

shareLink = (info, tab) ->
  # console.log("item " + info.menuItemId + " was clicked")
  # console.log("info: " + JSON.stringify(info))
  # console.log("tab: " + JSON.stringify(tab))
  shareWithTwuner(info.linkUrl)
  return

getActiveWindow = () ->
  views = chrome.extension.getViews()
  for v in views
    if v.location.href == root._twunerTab.url
      return v
  return null

showTwunerTab = () ->
  if root._twunerTab
    proc = (c) ->
      c.focused ? chrome.windows.update(c.id, {
        focused: true
      })
      return
    chrome.tabs.get(root._twunerTab.id, (c) ->
      root._twunerTab = c
      chrome.windows.get(c.windowId, proc)
    )
    chrome.tabs.update(root._twunerTab.id, {
      selected: true
    })
  return

shareWithTwuner = (str) ->
  _doShare = () ->
    win = getActiveWindow()
    _testProc = () ->
      if win and win.globals
        if win.globals.signed_in
          win.ui.StatusBox.change_mode(win.ui.StatusBox.MODE_TWEET)
          win.ui.StatusBox.set_status_text(str)
          win.ui.StatusBox.open()
        else
          try
            win.toast.set('You must sign in to share content.').show(-1)
          catch e
            setTimeout(_testProc, 1000)
      else
        setTimeout(_testProc, 500)

    _testProc()

  if root._twunerTab and root._twunerTab.id
    showTwunerTab()
    _doShare()
  else
    chrome.tabs.create(
      { url: "index.html" },
      -> setTimeout(_doShare, 500)
    )
  return

onTabCreated = (tab) ->
  tabChangedHandler(tab)
  return

onTabUpdated = (id, info, tab) ->
  tabChangedHandler(tab)
  return

onTabRemoved = (id, info) ->
  if root._twunerTab
    if root._twunerTab.id == id
      root._twunerTab = null
  return

onExtRequest = (req, sender, response) ->
  if req.enableContextMenu
    install()
    response({'reply': 'getcha, context menu has been enabled.'})
  else
    uninstall()
    response({'reply': 'getcha, context menu has been disabled.'})
  return

install = () ->
  # console.log('install')
  contexts = ["page","selection","link"]
  if root._menuItemSharePageId == null
    root._menuItemSharePageId = chrome.contextMenus.create({"title": "Share Page with Twuner", "contexts":["page"], "onclick": sharePage})
  if root._menuItemShareSelId == null
    root._menuItemShareSelId = chrome.contextMenus.create({"title": "Share Selection with Twuner", "contexts":["selection"], "onclick": shareSelection})
  if root._menuItemShareLinkId == null
    root._menuItemShareLinkId = chrome.contextMenus.create({"title": "Share Link with Twuner", "contexts":["link"], "onclick": shareLink})
  return

uninstall = () ->
  # console.log('uninstall')
  chrome.contextMenus.removeAll()
  root._menuItemSharePageId = null
  root._menuItemShareSelId = null
  root._menuItemShareLinkId = null
  if chrome.tabs.onCreated.hasListener(onTabCreated)
    chrome.tabs.onCreated.removeListener(onTabCreated)
  if chrome.tabs.onUpdated.hasListener(onTabUpdated)
    chrome.tabs.onUpdated.removeListener(onTabUpdated)
  return

chrome.tabs.onCreated.addListener(onTabCreated)
chrome.tabs.onUpdated.addListener(onTabUpdated)
chrome.tabs.onRemoved.addListener(onTabRemoved)
chrome.extension.onRequest.addListener(onExtRequest)

root = exports ? this
root._twunerTab = null
root._install = install
root._uninstall = uninstall
root._menuItemSharePageId = null
root._menuItemShareSelId = null
root._menuItemShareLinkId = null

