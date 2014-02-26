Readme.txt

Compilation 

Pour compiler tous les logiciels, il faut exécuter makedzb.sh. On obtient trois fichiers à lancer: «Dazibao» – le logiciel d' affichage/modification de Dazibao, «Notifier» – le logiciel qui envoie des notifications si le fichier .dzb a été modifié et «Listener» – le logiciel qui réagit aux notifications. 

«Dazibao»

Pour commencer à travailler avec le logiciel «Dazibao», il faut lancer le fichier «Dazibao». 
La fenêtre «Bienvenue» sera ouverte. Ici, on peut sélectionner le fichier Dazibao (.dzb) et mettre le checkbox en mode «Read only» en choisissant ainsi une manière de travailler avec Dazibao.
Le fichier dazibao sera ouvert et la liste des TLVs sera affichée. Vous pouvez choisir le TLV et l'afficher en double-cliquant sur ce fichier. 
Si le checkbox «Read-only» n’a pas été activé, le Dazibao sera ouvert en mode modification. Dans ce cas, on peut aussi consulter le contenu de Dazibao, mais on peut également y ajouter et supprimer des posts. Pour ajouter  un post, il faut appuyer sur le bouton «Add» dans la fenêtre principale. La fenêtre de création de post sera ouvert. Ici, on peut taper un texte, ajouter une image PNG ou JPEG ou une animation GIF et ajouter la date en sélectionnant sur le checkbox l'option «Set date». Pour conserver le post, appuyez sur «Save», pour annuler, appuyez sur «Cancel». 
Il est possible de supprimer des posts sur Dazibao. Pour faire ça, sélectionnez le post à supprimer dans la liste et appuyez sur «Delete». Il est possible de supprimer plusieurs posts en une seule fois. Pour faire cela, choisissez plusieurs posts en utilisant le bouton «shift» ou «ctrl». Appuyez sur le bouton «Delete».

«Notifier» et «Listener»

Lancez tout d’abord «Listener». Puis, lancez «Notifier» en passant comme des paramètres les fichiers Dazibao (*.dzb) dont la modification vous intéresse (par exemple, «./Notifier ./dazibao1.dzb ./dazibao2.dzb ./dazibao3.dzb»). Puis essayez de modifier le fichier Dazibao passé comme un paramètre à travers le logiciel «Dazibao». Les notifications seront affichées sur l’output standard de «Listener» et comme une notification Gnome.
