import traceback
import socket
 
UDP_BREAK = 5.0
UDP_DEF_PORT = 49502
UDP_DEF_ADDRESS = ""
UDP_RCV_SIZE = 1024
 
def udp_run( dct):
    """Run UDP message loop forever"""
 
    port = dct.get('ipport', UDP_DEF_PORT )
    address = dct.get('address', UDP_DEF_ADDRESS )
 
    try:
        # attempt to bind on port linked to the Digi IA Modbus bridge
        udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        udpSock.bind((address, port))
        udpSock.settimeout( UDP_BREAK)
 
    except:
        return
 
    while True:
 
        # because we don't block, your code can do periodic other things
 
        try:
            req, addr = udpSock.recvfrom( UDP_RCV_SIZE)
 
        except socket.timeout: # this is hit every 5 seconds (or as you configured)
            continue
 
        except: # all other errors, we just print out details and continue
            traceback.print_exc()
            break
 
        if(len(req)>0):
            # then we received something - should always be true if here
 
            # SEND IT TO USER
            rsp_tuple = send_request_to_user( (req, addr, dct) )
 
            if(rsp_tuple and (len(rsp_tuple[0]) > 0)):
                # then user returned a response as (rsp,addr,dct)
                rsp = rsp_tuple[0]
                numBytes = udpSock.sendto( rsp, addr)
            # else no response desired
 
        req = ""
        rsp = ""
        rsp_tuple = ()
 
    # endwhile(true)
 
    udpSock.close()
 
    return
 
def send_request_to_user( req_tuple):
    """Given Modbus/TCP message, return exception response bad-data
 
    Keyword arguments:
    req_tuple = the request/address pair
      req_tuple[0] = binstr of the protocol request
      req_tuple[1] = address from socket (TCP or UDP)
      req_tuple[2] = dictionary of options
 
    Return form:
    rsp_tuple = the response/address pair
      if None, ia_server will hang-up
      rsp_tuple[0] = binstr of the protocol response ("" is okay - no rsp)
      rsp_tuple[1] = address from req_tuple[1]
      rsp_tuple[2] = dictionary of options
 
    """
 
    msg = req_tuple[0]
    addr = req_tuple[1]
    dct = req_tuple[2]
 
    # print 'dummy mbtcp_responder saw addr ', addr
 
    # we'll throw excpetion if not types.StringTypes
 
    if( len(msg) < 8):
        # header is too short to know full length
        return None
 
    # Here we have full header, format = SS SS 00 00 LL LL
    # SS SS is sequence number - can be anything
    # 00 00 is protocol format, should be 0x0000 or protocolver
    # LL LL is the length, which we allow to be up to 0xFFFF
 
    rsp = msg[:4]
    rsp += chr(0)
    rsp += chr(3)
    rsp += msg[6] # old unit id
    rsp += chr(ord(msg[7])|0x80) # old func made to error
    rsp += chr(2)
 
    return (rsp,addr,dct)
