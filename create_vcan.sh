#!/bin/bash

sudo ip link add dev vcan0 type vcan 
sudo ifconfig vcan0 up
