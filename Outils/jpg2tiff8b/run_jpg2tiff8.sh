#!/bin/bash

#-----------------------------------------------------------------------
#
#   Script bash permettant de lancer la conversion jpeg vers tiff
#   sur une serie d'images dans un repertoire
#
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
#---- Usage
#-----------------------------------------------------------------------
function usage
{
    echo "Usage : ./run_jpg2tiff8b.sh -i <Repertoire des images JPG>"
    echo "                            -o <Repertoire de sortie>"

}

HERE=$(dirname $0)

#-----------------------------------------------------------------------
#---- PROCESS
#-----------------------------------------------------------------------

#---- Gestion des arguments
if [ $# -ne 4 ]
then
    usage
    exit 2
fi


while getopts i:o: option
do
    case $option in
        i)
            INPUT_DIR="$OPTARG"
            ;;
        o)
            OUTPUT_DIR="$OPTARG"
            ;;
        ?)
            usage
            exit 2
            ;;
    esac
done


#---- Liste des images
image_list=$(ls $INPUT_DIR | grep -i .jpg )


#--- Nombre d'images' a traiter
nb_image=$(echo $image_list | wc -w)
echo "INFO   : $nb_image images a traiter"


#---- Traitement des images
index_image=1
for image in $image_list
do
    
    echo "INFO   : Traitement de l'image $image ($index_image / $nb_image)"
    
    tif_file_name=$(basename $image .${image##*.}).tif
    
    ./jpg2tiff8b ${INPUT_DIR}/${image} ${OUTPUT_DIR}/${tif_file_name}

    if [ $? -ne 0 ]
    then
        echo "ERREUR : Echec de la conversion de l'image ${INPUT_DIR}/${image}"
    fi
    
    index_image=$(($index_image + 1))
    
done
