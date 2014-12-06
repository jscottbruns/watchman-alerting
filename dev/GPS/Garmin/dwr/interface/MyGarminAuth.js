HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
Vary: Accept-Encoding
Content-Length: 1283
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

if (MygarminAuth == null) var MygarminAuth = {};
MygarminAuth._path = '/locate/dwr';
MygarminAuth.login = function(p0, p1, callback) {
  dwr.engine._execute(MygarminAuth._path, 'MygarminAuth', 'login', p0, p1, callback);
}
MygarminAuth.getUsername = function(callback) {
  dwr.engine._execute(MygarminAuth._path, 'MygarminAuth', 'getUsername', callback);
}
MygarminAuth.logout = function(callback) {
  dwr.engine._execute(MygarminAuth._path, 'MygarminAuth', 'logout', callback);
}
MygarminAuth.isAuthenticated = function(callback) {
  dwr.engine._execute(MygarminAuth._path, 'MygarminAuth', 'isAuthenticated', callback);
}
