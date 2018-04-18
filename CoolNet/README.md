# Project #

This project is based on the paper [Making a long story short: A Multi-Importance Semantic for Fast-Forwarding Egocentric Videos](http://www.verlab.dcc.ufmg.br/link_to_the_file.pdf) on the __Special Issue on Egocentric Vision and Lifelogging Tools__ at __Journal of Visual Communication and Image Representation
__ (EVLT@@JVCI 2017).

CoolNet, a network model used to rate the frame “coolness” based on web
video statistics.

For more information, acess the link: http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction.

## Contact ##

### Authors ###

* Michel Melo da Silva - PhD student - UFMG - michelms@dcc.ufmg.com
* Washington Luis de Souza Ramos - MSc student - UFMG - washington.ramos@outlook.com
* Felipe Cadar Chamone - Undergraduate Student - UFMG - cadar@dcc.ufmg.br
* João Pedro Klock Ferreira - Undergraduate Student - UFMG - jpklock@ufmg.br
* Mario Fernando Montenegro Campos - Advisor - UFMG - mario@dcc.ufmg.br
* Erickson Rangel do Nascimento - Advisor - UFMG - erickson@dcc.ufmg.br

### Institution ###

Federal University of Minas Gerais (UFMG)  
Computer Science Department  
Belo Horizonte - Minas Gerais -Brazil 

### Laboratory ###

__VeRLab:__ Vison and Robotic Laboratory  
http://www.verlab.dcc.ufmg.br

## Code ##

### Dependencies ###

* Python 2.7 _(Tested with 2.7.12)_
* OpenCV 2.4 _(Tested with 2.4.9 and 2.4.13)_
* Caffe 1.0 _(Tested with 1.0.0-rc5)_ 

## Classifier ##

* Download our pretrained caffemodel [512MB]

```
  sh Scripts/download_model.sh
```

* Convert the mean file from .binaryproto to .npy using :
```
  python convert_mean.py <path/to/mean.binaryproto>  
```
* Use the script  ` Scripts/classifier.py ` to generate the scores for each frame of the video

```bash
  python classifier.py <video> <caffemodel> <deploy> <mean> [flags]
```


| Options      | Description                | Type   | Example |
|-------------:|----------------------------------|--------|---------|
|`<video>`     | Input video to predict           | String | `../Example/example.mp4` |
|`<caffemodel>`| CoolNet weights                  | String | `model/coolnet_pretrained.caffemodel`
|`<deploy>`    | Network Architecture             | String | `model/coolnet_deplor.prototxt`
|`<mean>`      | Mean file                        | String | `model/train_mean.npy`|
|`[flags]`     | Set CPU or GPU prediction        | String | `-cpu` _(default)_ or `-gpu`|


#### Output file ####

The output file from the classifier script contains the probability of each frame to be cool.

```text
frame_number, cool_score
frame_number, cool_score
frame_number, cool_score
frame_number, cool_score
...
```


#### Try it out: ####

```bash
$    cd ~
$    git clone https://radac98@bitbucket.org/semantichyperlapse/semantic_first_person_videos_hyperlapse_jvci-2017.git
$    cd semantic_first_person_videos_hyperlapse_jvci-2017/CoolNet/
$    python Scripts/convert_mean.py model/train_mean.binaryproto
$    python Scripts/classifier.py ../Example/example.mp4 model/coolnet_pretrained.caffemodel  model/coolnet_deploy.prototxt model/train_mean.npy -cpu
```

## Acknowledgements ##

1.  __BVLC_Caffe__ : Jia, Yangqing and Shelhamer, Evan and Donahue, Jeff and Karayev, Sergey and Long, Jonathan and Girshick, Ross and Guadarrama, Sergio and Darrell, Trevor, in: Caffe: Convolutional Architecture for Fast Feature Embedding, arXiv preprint arXiv:1408.5093, 2014.
2.  __VGG_Places__ : B. Zhou, A. Lapedriza, J. Xiao, A. Torralba, A. Oliva, Learning deep features for scene recognition using places database, in: Advances in Neural Information Processing Systems 27, Curran Associates, Inc., 2014, pp. 487Ã¢â‚¬â€œ495.

## Citation ##

If you are using it to academic purpose, please cite: 

M. M. Silva, W. L. S. Ramos, F.C. Chamone, J. P. K. Ferreira, M. F. M. Campos, E. R. Nascimento, __Making a long story short: A Multi-Importance Semantic for Fast-Forwarding Egocentric Videos__, in: Special Issue on Egocentric Vision and Lifelogging Tools at Journal of Visual Communication and Image Representation. doi:XXX/XXX.

### Bibtex entry ###

> @ article{Silva2017,  
>            Title      = {Making a long story short: A Multi-Importance Semantic for Fast-Forwarding Egocentric Videos},  
>            Author     = {Silva, Michel Melo and Ramos, Washington Luis Souza and Chamone, Felipe Cadar and Ferreira, Joao Pedro Klock and Campos, Mario Fernando Montenegro and Nascimento, Erickson Rangel},  
>            Year       = {2017},  
>            Journal    = {Journal of Visual Communication and Image Representation},
>			 Issuetitle = {Special Issue on Egocentric Vision and Lifelogging Tools},
>            Doi        = {XXX/XXX},    
> }

###### Enjoy it. ######