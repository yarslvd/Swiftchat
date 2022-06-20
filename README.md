# UChat

Uchat is our team program which is made to show how chat could be user friendly and include some interesting features, which differs it among others.

## Instruction
Before running the program you should write some commands in terminal and do some monipulations
1. First of all, we should install brew
```bash 
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Run script 
```bash
zsh install.sh
```
3. After this manipulations we should download GTK 4
```bash
# for average user
brew install gtk4

# for campus user
brew install gtk+3

brew install gtk4

```
4. Finaly get to compiling and running the program

```bash
make
./uchat_server 888
./uchat 127.0.0.1 888
```
## Implementation
Our code is a complex of visual part, databases and backend. 
For visual part was used [GTK 4](https://docs.gtk.org/gtk4/): 
```c
#include <gtk/gtk.h>
```
Databases are written with the help of databases engine [SQlite3](https://www.googleadservices.com/pagead/aclk?sa=L&ai=DChcSEwiI99OH3-_1AhVUstUKHS7CAWwYABACGgJ3cw&ae=2&ohost=www.google.com&cid=CAASEuRo1K12ogk7jcTVNS-YeW7hAQ&sig=AOD64_0qaH_-koiDzKlhx-xwNCZYfZWJ6g&ved=2ahUKEwj8-cyH3-_1AhXHPOwKHRLyDmAQqyQoAXoECAMQBg&dct=1&adurl=)
```c
#include "../../frameworks/SQLite3/inc/sqlite3.h"
```
Safe connection is provided by [SSL 1.1.1](https://www.cloudflare.com/en-gb/lp/ssl-b/?&_bt=523616199274&_bk=ssl&_bm=p&_bn=g&_bg=123092899416&_placement=&_target=&_loc=1012866&_dv=c&awsearchcpc=1&gclid=Cj0KCQiAxoiQBhCRARIsAPsvo-y0CkwDaPTE7iZnwiF-GfoCN5_xLmXZ6VeAN6DWBEc5n6oltX_dCoYaAuJiEALw_wcB&gclsrc=aw.ds)
```c
#include <openssl/ssl.h>
#include <openssl/err.h>
```

Precisely because of the large amount of code, we cannot register absolutely everything, but we will leave a link to [GIT](https://github.com/kevandee/uchat.git) with all the available code

## Helpful links
Weather icons were made by [Roman Káčerek](https://www.flaticon.com/authors/roman-kacerek) and have been adapted from this site: <https://www.flaticon.com/packs/weather-407>

Concept of our chat: <https://www.figma.com/file/cYoYKe1oJApM1YqCESteyq/uchat?node-id=77%3A993>

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
