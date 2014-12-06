HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
Vary: Accept-Encoding
Content-Length: 1653
Content-Type: text/plain;charset=ISO-8859-1
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close


if (typeof MygarminDeviceBean != "function") {
  function MygarminDeviceBean() {
    this.model = null;
    this.unitId = null;
  }
}

if (typeof MygarminPoiBean != "function") {
  function MygarminPoiBean() {
    this.postalCode = null;
    this.phoneNumber = null;
    this.name = null;
    this.countryCode = null;
    this.stateCode = null;
    this.address1 = null;
    this.longitude = 0;
    this.address2 = null;
    this.latitude = 0;
    this.note = null;
    this.city = null;
  }
}

// Provide a default path to dwr.engine
if (dwr == null) var dwr = {};
if (dwr.engine == null) dwr.engine = {};
if (DWREngine == null) var DWREngine = dwr.engine;

if (ManageLocations == null) var ManageLocations = {};
ManageLocations._path = '/locate/dwr';
ManageLocations.saveSingleWaypointToMyGarmin = function(p0, callback) {
  dwr.engine._execute(ManageLocations._path, 'ManageLocations', 'saveSingleWaypointToMyGarmin', p0, callback);
}
ManageLocations.saveWaypointsToMyGarmin = function(p0, callback) {
  dwr.engine._execute(ManageLocations._path, 'ManageLocations', 'saveWaypointsToMyGarmin', p0, callback);
}
ManageLocations.getDeviceList = function(callback) {
  dwr.engine._execute(ManageLocations._path, 'ManageLocations', 'getDeviceList', callback);
}
ManageLocations.sendSingleWaypointWirelessly = function(p0, p1, p2, callback) {
  dwr.engine._execute(ManageLocations._path, 'ManageLocations', 'sendSingleWaypointWirelessly', p0, p1, p2, callback);
}
ManageLocations.sendWaypointsWirelessly = function(p0, p1, p2, callback) {
  dwr.engine._execute(ManageLocations._path, 'ManageLocations', 'sendWaypointsWirelessly', p0, p1, p2, callback);
}
