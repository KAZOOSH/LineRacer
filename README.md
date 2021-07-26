2019_LineRacer



## Install Epson TM Driver Linux


Download [Epson TM Linux Driver](https://download.epson-biz.com/modules/pos/index.php?page=single_soft&cid=6408&scat=32&pcat=3).

```
sh build.sh
sudo sh install.sh
```

add in cups (or printer settings):

```
http://localhost:631/
```

* add new printer
* name is ```unkwown```
* Driver is Epson -> EPSON TM Thermal (180dpi)


printer not detected :

sudo chmod og= /usr/lib/cups/backend/usb