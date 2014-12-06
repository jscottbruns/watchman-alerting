function init_tabcontent() {
	var nav = new ddtabcontent("mainnav");
	nav.setpersist(true);
	nav.setselectedClassTarget("link");
	nav.init();
}

function file_selector(){

	var el_1 = arguments[0];
	var el_2 = arguments[1];
	if ( $( el_1 ).style.display == 'block' ) {
        $( el_1 ).style.display = 'none';
        $( el_2 ).style.display = 'block';
    } else {
        $( el_1 ).style.display = 'block';
        $( el_2 ).style.display = 'none';
    }
}

function init_validator() {

    var f = arguments[0];

    var valid = new Validation(f);
    Validation.add('validate-ip', 'Not a valid IP address', function(v) {
		var octet = '(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])';
		var ip    = '(?:' + octet + '\\.){3}' + octet;
		var quad  = '(?:\\[' + ip + '\\])|(?:' + ip + ')';
		var ipRE  = new RegExp( '(' + quad + ')' );

        return ipRE.test( v );
    } );
	Validation.add('validate-mtu', 'Maximum transmission unit (MTU) is required and must be between 1 and 65536', {
        min : 1,
		max : 65536,
		include : [ 'required', 'validate-digits' ]
    } );
	Validation.add('validate-ms', 'Please enter a valid capture timeout range, or -1 to avoid using a timeout', {
        min : -1,
		isNot : 0,
		include : [ 'required', 'validate-number' ]
    } );
	Validation.add('validate-secs', 'Invalid signboard reset time. Must be no less than 30 seconds and not greater than 300 seconds (5 mins).', {
        min : 30,
		max : 300,
		include : [ 'required', 'validate-digits' ]
    } );
    Validation.add('validate-length', 'Valid comment length must not be less than 15 or greater than 300 characters', {
        min : 15,
        max : 300,
        include : [ 'validate-digits' ]
    } );
	Validation.add('validate-sleep-time', 'Sleep time must be no less than 5 seconds and no greater than 60 seconds', {
        min : 5,
		max : 60,
		include : [ 'validate-digits' ]
    } );
	Validation.add('validate-miltime-range', 'Time range must be specified as start-end (0600-1500). All times are specified in 24 hour format', function(v) {
        var re = /(?:(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23)[:]?(0|1|2|3|4|5)\d{1})-(?:(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23)[:]?(0|1|2|3|4|5)\d{1})/;
		return re.test(v);
    } );
	Validation.add('validate-short-date', 'Invalid date specified. Dates must be formatted as mm/dd and be valid calendar dates.', function(v) {
        var d = v.split(',');
		var re = /(0[1-9]|1[0-2])\/(0[1-9]|1[0-9]|2[0-9]|3[0-1])/;
		for (var i = 0; i < d.length; i++) {
            if (re.test(d[i])) {
                var mo = parseInt(RegExp.$1);
				var da = parseInt(RegExp.$2);
				if (mo == 4 | mo == 6 || mo == 9 || mo == 11) {
					if (da > 30) {
						return false;
					}
				} else if (mo == 2) {
                    if (da > 29) {
                        return false;
                    }
                }
			}
			else {
    			return false;
			}
		}
		return true;
    } );
	Validation.add('validate-minutes', 'Valid lookback time cannot be less than 5 or greater than 60 minutes', {
        min : 5,
		max : 60,
		include : [ 'validate-digits' ]
    } );
}