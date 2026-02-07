# f1ssf_rotator_prod
Il s'agit ici d'un systeme de pilotage site et azimut + 8 relais fonctionnel mais experimental.

Pilotage moteurs rotor en 24V AC 2 phases + 8 relais (module optocouplé tout cablé)

Calibration des rotors facile directement à l'ecran, avec mise en memoire de la calibration.

Je mets aussi en memoire la derniere position connue.

La recopie est analogique. Ce systeme peut s'interfacer avec tous les rotors analogiques connus.

La particularité de ce systeme, c'est qu'il dispose d'un master, et d'un remote au pied du pylone.

J'utilise une liaison RS485 2 fils + 2 fils pour le 24V AC + blindage

Les alimentations continues sont fabriquées au pied du pylone.

Road map du projet:

Dans l'état le projet est fonctionnel, le code peut sans doute être optimisé.

Il reste a implementer: le pilotage par logiciels tiers, la possibilité d'utiliser des capteurs de recopies numeriques.

une fonction Parking, un clavier de saisie AZ/EL, et sans doute plein d'autres choses.....

<img width="483" height="322" alt="Capture d&#39;écran 2026-02-07 094934" src="https://github.com/user-attachments/assets/a60b6b28-c40d-4cb2-b5e5-4a2d81750ce3" />
<img width="482" height="322" alt="Capture d&#39;écran 2026-02-07 095026" src="https://github.com/user-attachments/assets/578bacec-56cc-4a0a-bac1-c3ec7b28ac22" />
<img width="481" height="323" alt="Capture d&#39;écran 2026-02-07 094952" src="https://github.com/user-attachments/assets/e7f969c3-8d6e-46c7-b4d5-b90bf6c786dc" />
<img width="484" height="321" alt="Capture d&#39;écran 2026-02-07 095042" src="https://github.com/user-attachments/assets/33fdc555-0b07-4dfc-8352-07a69717bf6d" />

MASTER

![22c6202a-230e-41dc-9dc3-e37e44837cca](https://github.com/user-attachments/assets/1db5b960-8e3a-4726-bd0a-888105513f29)
![3846b2e4-0288-44b1-87a9-b8bb71a1f665](https://github.com/user-attachments/assets/eb99cd7d-696b-4dfb-98a5-288854553967)

REMOTE

![a3bb1c07-3af1-4b68-92d7-d347d3149b8a](https://github.com/user-attachments/assets/01fa999d-2f90-477b-acbf-0865334b4727)

RELAIS et ALIMS (Input 24VAC - ouput 5V/12V/ 28DC

![baac3d30-d017-47e9-b751-e947ed9a9ef7](https://github.com/user-attachments/assets/817edeaf-f288-485f-9218-ed1009ed2b54)
![6307f397-fdf7-4e8c-b24a-9a26c9bd798a](https://github.com/user-attachments/assets/67df2b60-ed98-4d42-8fca-736916dbf203)


