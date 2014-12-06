HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
ETag: W/"2485-1297447410000"
Last-Modified: Fri, 11 Feb 2011 18:03:30 GMT
Vary: Accept-Encoding
Content-Length: 2485
Content-Type: text/javascript
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close

var mgTabNumOfPointsSend = 0;

function mgTabLoginByEnter(e) {
	// TODO will be enable in v3.0.x release part 2
	if (isEnterPressed(e)) {
		clickMgTabSaveToMgButton();
	} // end if
}

function clickMgTabSaveToMgButton() {

	// block UI
	jqBlockUi();
	
	MygarminAuth.isAuthenticated({
		callback: function(data) {
			if (data == true) {
				cbMyGarminTabLogin(true);
			}
			else {
				var username = document.getElementById("mgTabMgUsername");
				var password = document.getElementById("mgTabMgPassword");
				
				if (username.value.length == 0 || password.value.length == 0) {
					logout(true);
				}
				else {
					MygarminAuth.login(
							username.value,
							password.value,
							{
								callback: cbMyGarminTabLogin,
								errorHandler: dwrCbError,
								exceptionHandler: dwrCbException,
								textHtmlHandler: dwrCbTextHtml
							});
				} // end if
			} // end if
		},
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
}

function cbMyGarminTabLogin(data) {
	
	if (data == true) {
		
		loginUiHandler();
		
		mgTabNumOfPointsSend = 0;
		
		for (var i = 0; i < mgPoiArray.length; i = i + 1) {
			
			ManageLocations.saveSingleWaypointToMyGarmin(
					mgPoiArray[i],
					{
						callback: function(data) {
							
							mgTabNumOfPointsSend = mgTabNumOfPointsSend + 1;
							
							if (mgTabNumOfPointsSend == mgPoiArray.length) {
								// all points are sent
								document.getElementById("mgTabMgLogin").style.display =
									"none";
								document.getElementById("mgTabSaveMgComplete").style.display =
									"inline";
								
								// GATC
								try {
									pageTracker._trackEvent(gatcEC_funcTabs,
											gatcEA_funcTabs_saveMgComplete);
								} catch(err) {}
								
								// unblock UI
								jQuery.unblockUI();
							} // end if
						},
						errorHandler: dwrCbError,
						exceptionHandler: dwrCbException,
						textHtmlHandler: dwrCbTextHtml
					});
		} // end for
	}
	else {
		logout(true);
	} // end if
}

function logoutMyGarminTab() {
	document.getElementById("mgTabMgLoginInputs").style.display = "inline";
	document.getElementById("mgTabMgLogin").style.display = "inline";
	
	document.getElementById("mgTabSaveMgComplete").style.display = "none";
}

function loginMyGarminTab() {
	document.getElementById("mgTabMgLoginInputs").style.display = "none";
}
