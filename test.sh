#!/bin/bash

mv new.txt old.txt
sudo ./driver.out
raspi-gpio get > new.txt
diff old.txt new.txt -u --color=always