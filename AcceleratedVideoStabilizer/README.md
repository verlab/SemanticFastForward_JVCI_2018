
[![Version](https://img.shields.io/badge/version-1.0-brightgreen.svg)](http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction/#ECCVW2016)
[![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg)](LICENSE)

# Project #

This code is based on the paper [Towards Semantic Fast-Forward and Stabilized Egocentric Video](http://www.verlab.dcc.ufmg.br/semantic-hyperlapse/papers/Final_Draft_ECCVW_2016_Towards_Semantic_Fast_Forward_and_Stabilied_Egocentric_Videos.pdf) on the __First International Workshop on Egocentric Perception, Interaction and Computing__ at __European Conference on Computer Vision__ (EPIC@@ECCV 2016). The goal of the program is to stabilize a fast-forward version of a video, using the dropped frames to reconstruct distorted images during the processing. 

For more information, acess the [project page](http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction).

## Contact ##

### Authors ###

* Michel Melo da Silva - PhD student - UFMG - michelms@dcc.ufmg.com
* Washington Luis de Souza Ramos - MSc student - UFMG - washington.ramos@outlook.com
* João Pedro Klock Ferreira - Undergraduate Student - UFMG - jpklock@ufmg.br
* Mario Fernando Montenegro Campos - Advisor - UFMG - mario@dcc.ufmg.br
* Erickson Rangel do Nascimento - Advisor - UFMG - erickson@dcc.ufmg.br

### Institution ###

Federal University of Minas Gerais (UFMG)  
Computer Science Department  
Belo Horizonte - Minas Gerais -Brazil 

### Laboratory ###

![VeRLab](https://www.dcc.ufmg.br/dcc/sites/default/files/public/verlab-logo.png)

__VeRLab:__ Laboratory of Computer Vison and Robotics   
http://www.verlab.dcc.ufmg.br

## Code ##

### Dependencies ###

* OpenCV 2.4 _(Tested with 2.4.9 and 2.4.13)_
* Armadillo 6 _(Tested with 6.600.5 -- Catabolic Amalgamator)_
* Boost 1 _(Tested with 1.54.0 and 1.58.0)_
* Doxygen 1 _(for documentation only - Tested with 1.8.12)_

### Compiling ###

The program can be compiled using either `cmake` or `qmake` tools.

Into the project directory, run the following commands: 

            user@computer:<project_path>: mkdir build 
            user@computer:<project_path>: cd build
            user@computer:<project_path/build>: cmake ..
            user@computer:<project_path/build>: make
            user@computer:<project_path/build>: ./VideoStabilization < Settings_file > [-h] [Range_min] [Range_max]

### Exemples of usage ###

            user@computer:<project_path/build>: < Program_name > < Settings_file > [-h] [Range_min = 0 ] [Range_max = num_frames ]

Example 1: Run VideoStabilization using Help option.

            user@computer:<project_path/build>: ./VideoStabilization -h 

Example 2: Run VideoStabilization in the Experiment_1 processing the whole video. 
            
            user@computer:<project_path/build>: ./VideoStabilization Experiment_1.xml

Example 3: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the last one. 

            user@computer:<project_path/build>: ./VideoStabilization Experiment_1.xml 150 

Example 4: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the frame 490. 

            user@computer:<project_path/build>: ./VideoStabilization Experiment_1.xml 150 490

### Documentation ###

The Accelerated Video Stabilizer documention can be accessed through the [link](http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction/doc/acceleratedVideoStabilizer).

To generate the documentation of the project, run the following command into the project folder:

            user@computer:<project_path>: doxygen doc/doc_conf
            
The documentation will be generated in the `<project_path>/doc` folder. 

## Citation ##

If you are using it to academic purpose, please cite: 

M. M. Silva, W. L. S. Ramos, J. P. K. Ferreira, M. F. M. Campos, E. R. Nascimento, __Towards semantic fast-forward and stabilized egocentric videos__, in: _European Conference on Computer Vision Workshops_, Springer International Publishing, Amsterdam, NL, 2016, pp. 557–571. doi:10.1007/978-3-319-46604-0_40.

### Bibtex entry ###

> @InBook{Silva2016,  
>            Title     = {Towards Semantic Fast-Forward and Stabilized Egocentric Videos},  
>            Author    = {Silva, Michel Melo and Ramos, Washington Luis Souza and Ferreira,Joao Pedro Klock and Campos, Mario Fernando Montenegro and Nascimento, Erickson Rangel},  
>            Editor    = {Hua, Gang and J{\'e}gou, Herv{\'e}},  
>            Pages     = {557--571},  
>            Publisher = {Springer International Publishing},  
>            Year      = {2016},  
>            Address   = {Cham},  
>            Booktitle = {Computer Vision -- ECCV 2016 Workshops: Amsterdam, The Netherlands, October 8-10 and 15-16, 2016, Proceedings, Part I},  
>            Doi       = {10.1007/978-3-319-46604-0_40},  
>            ISBN      = {978-3-319-46604-0},  
>            Url       = {http://dx.doi.org/10.1007/978-3-319-46604-0_40}  
> }

###### Enjoy it. ######
