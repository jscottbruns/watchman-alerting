HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
ETag: W/"330-1297447410000"
Last-Modified: Fri, 11 Feb 2011 18:03:30 GMT
Vary: Accept-Encoding
Content-Length: 330
Content-Type: text/javascript
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close

function sendMsnDirect() {

	// google-analytics
	try {
		pageTracker._trackEvent(gatcEC_msn, gatcEA_msn_sendComplete);
	} catch(err) {}
	
	document.getElementById("divSendByMsn").style.display = "none";
	document.getElementById("divMsnSendComplete").style.display = "inline";
	document.forms['frmMsnDirect'].submit();
}