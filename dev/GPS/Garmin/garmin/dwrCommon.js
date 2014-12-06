HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
ETag: W/"3330-1297447410000"
Last-Modified: Fri, 11 Feb 2011 18:03:30 GMT
Vary: Accept-Encoding
Content-Length: 3330
Content-Type: text/javascript
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close

function dwrCbException(errorString, exception) {
	
	logout();
	
	if ("UserNotLogin" == errorString) {
		alert(i18nActionAborted + "\n" + i18nErrDesc403);
	}
	else if("DeviceNotInList"	== errorString ||
			"ParamInvalid"		== errorString ||
			"GemsInternalError"	== errorString) {
		document.location = error500Page;
	}
	else {
		alert(i18nActionAborted + "\n" + errorString);
		document.location = error500Page;
	} // end if
}

function dwrCbTextHtml(errorString, exception) {
	
	logout();
	
	alert(i18nActionAborted + "\n" + errorString);
	document.location = error500Page;
}

function dwrCbError(errorString, exception) {
	
	logout();
	
	alert(i18nActionAborted + "\n" + errorString);
	document.location = error500Page;
}

function clearLoginInputs() {
	
	// wireless
	document.getElementById("wlsTabMgUsername").value = "";
	document.getElementById("wlsTabMgPassword").value = "";
	
	/* TODO will be enable in v3.0.x release part 2
//	// usb
//	document.getElementById("usbMgUsername").value = "";
//	document.getElementById("usbMgPassword").value = "";
//
//	// mygarmin
//	document.getElementById("mgTabMgUsername").value = "";
//	document.getElementById("mgTabMgPassword").value = "";
	*/
}

function displayMgLoginError(display) {
	
	// wireless
	document.getElementById("wlsTabMgLoginErr").style.display = display;
	
	/* TODO will be enable in v3.0.x release part 2
//	// usb
//	document.getElementById("divUsbMgLoginErr").style.display = display;
//	
//	// mygarmin
//	document.getElementById("mgTabMgLoginErr").style.display = display;
	*/
	
	// GATC
	if ("inline" == display) {
		try {
			pageTracker._trackEvent(gatcEC_funcTabs, gatcEA_funcTabs_invalidMgLogin);
		} catch(err) {}
	} // end if
}

function logout(isShowError) {
	
	MygarminAuth.logout({
		callback: function(data) {
			document.getElementById("signedInUser").innerHTML = i18nNotSignedIn;
			document.getElementById("headerLogoutLink").style.display = "none";
			clearLoginInputs();
			
			if (isShowError == true) {
				displayMgLoginError("inline");
			}
			else {
				displayMgLoginError("none");
			} // end if
			
			// handle tabs separately
			
			/* TODO will be enable in v3.0.x release part 2
//			logoutMyGarminTab();
//			logoutUsbTab();
			*/
			logoutWirelessTab();
			
			// unblock UI
			jQuery.unblockUI();
		},
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
}


function loginUiHandler() {
	
	MygarminAuth.getUsername({
		callback: function(data) {
			document.getElementById("signedInUser").innerHTML =
				i18nWelcomeUser.replace("{0}", data);
			document.getElementById("headerLogoutLink").style.display = "inline";
			
			clearLoginInputs();
			displayMgLoginError("none");
			
			// handle tabs separately
			
			/* TODO will be enable in v3.0.x release part 2
//			loginMyGarminTab();
//			loginUsbTab();
			*/
			loginWirelessTab();
		},
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
}

function isEnterPressed(e) {
	
	var keycode = -1;
	
	if (window.event) {
		keycode = window.event.keyCode;
	}
	else if (e) {
		keycode = e.which;
	} // end if

	return (keycode == 13);
}
