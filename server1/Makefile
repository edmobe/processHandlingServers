# Copyright 2018 Pedro Melgueira
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

all:
	cd src && make
	cd tests && make

check:
	cd tests && make check

run:
	# convert imgs/img.png imgs/img.rgb
	./src/sobel imgs/0img1080.rgb imgs/img_out.gray 1920x1080
	#-i imgs/img_out_h.gray imgs/img_out_v.gray
	convert -size 1920x1080 -depth 8 imgs/img_out.gray imgs/img_out1.png

clean:
	cd src && make clean
	cd tests && make clean

clean_run:
	rm imgs/img_out* imgs/img.rgb



# SHELL   = /bin/bash
# BASEDIR = '/cvolume'

# make_env:
# 	mkdir -p $(HOME)$(BASEDIR)/{R,G,B,Not_trusted,temp}
# # Getting the config file
# ifeq (,$(wildcard ./configuracion.config))
# 	wget https://raw.githubusercontent.com/kisung5/Tarea1SOs/master/server-c/configuracion.config
# endif
# # This is to build the project image, not needed really	
# #	sudo docker build -t centos-server .
# # This is for running directly the server

# run:
# 	sudo docker run -p 9090:9090 -it -v $(HOME)$(BASEDIR):/volume --env-file configuracion.config  ksnonsteal/centos-imagecserver

# rmvolume:
# 	sudo rm -r $(HOME)/cvolume

# clean:
# 	sudo rm -r $(HOME)/cvolume
# 	sudo rm configuracion.config
