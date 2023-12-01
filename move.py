import os
import shutil
import glob

def deplacer_fichiers(dossier_source, dossier_destination):
    if not os.path.exists(dossier_destination):
        os.makedirs(dossier_destination)
    
    fichiers_source = glob.glob(os.path.join(dossier_source, '*'))
    
    for fichier in fichiers_source:
        shutil.move(fichier, dossier_destination)

def deplacer_dossiers_principaux(dossier_parent):
    dossiers_inputs = glob.glob(os.path.join(dossier_parent, "inputs", "20*"))
    dossiers_src = glob.glob(os.path.join(dossier_parent, "src", "20*"))

    for dossier in dossiers_inputs + dossiers_src:
        if os.path.isdir(dossier):
            annee = os.path.basename(dossier)
            
            deplacer_fichiers(os.path.join(dossier_parent, "inputs", annee), os.path.join(dossier_parent, annee, "inputs"))
            deplacer_fichiers(os.path.join(dossier_parent, "src", annee), os.path.join(dossier_parent, annee, "src"))

# Dossier parent est le dossier du script
dossier_parent = os.path.dirname(os.path.abspath(__file__))

deplacer_dossiers_principaux(dossier_parent)