# bluez-rfcomm-example
An example to register an RFCOMM server using my dbus-mock library.
But this is useful for everyone trying to implement a bluez serial service.
You can use the sdbus++ library, or any dbus lib. The code should be 
understandable enough for that.

This provides a simplified example project from which a more powerful and 
useful server can be build upon.

Since getting to this point in C++ was an absolute nightmare, I decided to share my efforts here.

Could contain bugs.

### What have I learned.
Please dont laugh, the documentation is just digustingly lacking.
1) The bluez documentation is a HUGE pain.
2) The sdbus documentation is a HUGE pain.
3) The name of all interfaces registered with "sd_bus_add_object_vtable" are gathered under the given sd_bus* instance. This instance can be named with "sd_bus_request_name" (which has no documentation). The naming is possible without any further work on the session (user) bus, but requires special permissions on the system bus. https://stackoverflow.com/questions/32828468/sd-bus-api-sd-bus-request-name-returns-permission-denied

4) Despite all the horribly outdated sources, creating and binding your own socket is not needed. Bluez does that for you. Scrap all that legacy code and just use the dbus methods.

5) In order to create a rfcomm server, you have to first register a new interface on the bus with a given object path and the type "org.bluez.Profile1". The "NewConnection" function is called whenever a new connection is made. You then take control of the file descriptor and handle read/writes from there.

6) To tell bluez that you just registered/exposed this new Profile1 interface, you have to call "RegisterProfile". this method takes an object_path (to the "Profile1" interface of yours that you created), A uuid and options.
this uuid cannot be chosen willy nilly. I assume that the client connects to a specific id. But I haven't implemented a client yet (just used apps). Use the SerialPortServiceClass uuid if you want to be compatible.
The function also takes a dictionary with options, for which the documentation is somewhat sufficient for once: https://github.com/pauloborges/bluez/blob/master/doc/profile-api.txt

7) I dont know if the passed channel matters at all. In my testing with common android apps, it doesn't. I dont know how the handle their connect.

### Building
I tend to have workspaces with all projects and dependencies in parallel with a "build" directory in each. This reproduces my workflow and lets cmake find the dependencies:

- mkdir bluetooth_project
- cd bluetooth_project
- git clone https://github.com/5cript/dbus-mockery.git
- git clone https://github.com/5cript/dbus-mockery-system.git
- git clone https://github.com/5cript/bluez-rfcomm-example.git
- cd dbus-mockery
- mkdir build
- cd build
- cmake ..
- make -j4
- cd ../../bluez-rfcomm-example
- mkdir build
- cd build
- cmake ..
- make -j4