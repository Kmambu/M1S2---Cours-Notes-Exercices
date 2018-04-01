# TP3 PERI - Pilotage d'un écran LCD en mode utilisateur et par un driver


## Objectifs
L'objectif de la séance est de:
* Configurer les GPIOs pour le pilotage d'un périphérique
* Comprendre le fonctionnement d'un écran LCD et ses fonctions de base
* Afficher du monitoring système
* Ecrire un driver d'affichage


## Étape 1 : Configuration des GPIO pour le pilotage de l'écran

### Configuration des GPIO

Il faudra configurer les GPIOs des signaux E, RS et Dx comme sortie pour permettre de les modifier par la suite, selon nos besoin.

### Ecriture des valeurs vers l'afficheur LCD
Pour afficher dans le LCD, on doit tout d'abord mettre la valeur du signal E à 0, puis on stocke dans les signaux D4 à D7 les bits de poids forts de la valeur qu'on veut afficher, puis on met E à 1 (pour enregistrer les valeurs de D4 à D7), ensuite on remet E à 0 et on stocke dans les signaux D4 à D7 les bits de poids faible de cette valeur.

### Initialisation de l'afficheur LCD
A partir de la documentation de *NewHeavenDisplay* (page 22), on conclut que l'afficheur doit recevoir les valeurs suivantes pour s'initialiser :
```bash
* 0x0011 0011 
* 0x0011 0010
* 0x0010 0000 #Function Set
	* 0x0000 0000 #Définir mode 4 bits
	* 0x0000 1000 #Définir 2 lignes d'affichage
	* 0x0000 0000 #Définir 40 points (5x8 dots)
* 0x0000 1000 #Contrôle d'affichage (display control)
	* 0x0000 0100 #Activer l'affichage (display on)
	* 0x0000 0000 #Désactiver le curseur (cursor off)
* 0x0000 0100 #Configuration de l'écriture
	* 0x0000 0000 #Entry mode : right (écriture vers la droite)
	* 0x0000 0000 #Display no shift : 
```

### Demande d'affichage d'un caractère
Pour demander l'affichage d'un caractère, il faut mettre RS à 1 (pour dire que la requête concerne l'envoi d'une donnée, et non pas d'une instruction). Ensuite on exécute la fonction qui permet d'écrire les valeurs vers l'afficheur.

### Envoi de commande
Pour envoyer des commandes, il faut mettre RS à 0, puis charger dans le LCD les valeurs adéquates à la requête demandée ( ainsi par exemple pour effacer l'écran, on envoie la valeur 0x0000 0001). Pour déplacer le curseur, on le verra ultérieurement dans ce rapport.

## Étape 2 : Fonctionnement de l'écran et fonctions de base

### lcd_strobe()
La fonction *lcd_strobe()* permet de faciliter les étapes de *set* et *reset* du signal E, avec un temps de latence de 1us pour bien prendre en compte la variation de sa valeur.
```cpp
void lcd_strobe(void)
{
  gpio_update(E,1);
  usleep(1);
  gpio_update(E,0);
}
```

### lcd_write4bits()
Comme son nom l'indique, cette fonction permet d'écrire dans les signaux D5 à D7 les bits de poids forts puis ceux de poids faibles représentant la valeur qu'on désire afficher sur le LCD.
Il est à noter qu'entre ces 2 étapes, il faudra changer la valeur de E pour pouvoir sauvegarder les bits donnés.
```cpp
void lcd_write4bits(int data)
{
  /* first 4 bits */
  gpio_update(D7, BIT(7,data));
  gpio_update(D6, BIT(6,data));
  gpio_update(D5, BIT(5,data));
  gpio_update(D4, BIT(4,data));
  lcd_strobe();
  /* second 4 bits */
  gpio_update(D7, BIT(3,data));
  gpio_update(D6, BIT(2,data));
  gpio_update(D5, BIT(1,data));
  gpio_update(D4, BIT(0,data));
  lcd_strobe();
  usleep(50); /* le délai minimum est de 37us */
}
```
### lcd_init()
Cette fonction permet d'initialiser l'afficheur LCD en lui envoyant les valeurs que nous avons cité précédemment :
```cpp
void lcd_init(void)
{
  gpio_update(E, 0);
  lcd_command(HEX(0011,0011)); /* initialization */
  lcd_command(HEX(0011,0010)); /* initialization */
  lcd_command(LCD_FUNCTIONSET    | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS );
  lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF );
  lcd_command(LCD_ENTRYMODESET   | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT );
}
```

### Programme d'affichage
Le programme principal de notre fichier *lcd_user.c* doit contenir l'appel vers la fonction *lcd_message(argv[1])* qui permet d'afficher  le message, qui sera contenu dans le 1er argument après l'appelation du fichier .x équivalent à notre .c </br>
Tout d'abord, on compile notre programme :
```bash
debbaghi@morales:lab3 1004:$ make
debbaghi@morales:lab3 1004:$ make upload
```
Le makefile est identique à celui utilisé durant les anciennes manipulations :
```makefile
RPI?=26
SRC=src
APP=lcd_user
DST=debbaghi-mambu/lab3
:
CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -L$(LIB) -lgpio

KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSSDIR        = /users/enseig/franck/peri
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

all:  $(APP).x upload

$(APP).x: $(APP).c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static

upload: 
	scp -P50$(RPI) $(APP).x pi@132.227.102.36:$(DST)

clean:
	rm -f *.o *.x *~
```

Une fois connecté dans la carte, il suffit d'exécuter l'instruction suivante:
```bash
pi@raspberrypi ~/debbaghi-mambu/lab3 $ sudo ./lcd_user.x "hello world"
```

### Manipulation du curseur
Le curseur peut être manipulé de diverses manières :
* Si le contenu qu'on veut afficher déborde, il serait intéressant de pouvoir afficher la totalité du contenu, même si ceci coûterait de l'écrire sur plusieurs lignes :
```cpp
void lcd_message(char* txt)
{
  int i, j, l = 0;
  for(i=0; i<strlen(txt); i++){
  	if(i%20 == 0 && i != 0) {
  		lcd_set_cursor(++l, 0);
  	}	
    lcd_data(txt[i]);
  }
}
```
* Si on veut afficher le même contenu sur plusieurs lignes, il suffit de le mentionner dans le programme principal :

```cpp
/* Display same text in the 4 lines */
  int i;
  for( i=0; i<4; i++)
  {
  lcd_set_cursor(i,0);
  lcd_message(argv[1]);
  }
```
Pour pouvoir faire tout ceci, il faudra configurer notre curseur, à travers la fonction *set_cursor()* :
```cpp
const int ddram[4] = {0x00, 0x40, 0x14, 0x54};
void lcd_set_cursor(int l, int c)
{
	lcd_command(LCD_SETDDRAMADDR + ddram[l%4] + c%0x14); 
    //14 en hexa est 20 en décimal (la longueur de notre afficheur)
}
```

## Etape 3 : Affichage du monitoring système
### Programme d'affichage du monitoring système
Afficher du monitoring revient à lire le contenu du fichier */proc/loadavg* , à partir des fonctions *read()* et *write()*
```cpp
  int fd;
  char buf[256];
  if((fd = open("/proc/loadavg", O_RDONLY, 0644)) == -1)
        return -1;
  while(1)
  {
  	read(fd, buf, 256);
  	lcd_message(buf);
  	sleep(1);
  	lseek(fd,0,SEEK_SET);
  	lcd_set_cursor(0,0);
  } 
```
En utilisant cette procédure, l'ensemble du contenu du fichier est affiché dans une seule ligne (ou plus s'il y a débordement).
</br>
Pour parser le contenu, on peut se baser sur les fonctions *sscanf()* et *sprintf()*:
```cpp
  int fd;
  char buf[256];
  double taba[3];
  int tabb[3];
  if((fd = open("/proc/loadavg", O_RDONLY, 0644)) == -1)
        return -1;
 while (1)
 {  
   int n = read (fd, buf, 256);
   sscanf(buf, "%lf %lf %lf %d/%d %d\n", &taba[0],&taba[1],&taba[2],&tabb[0],&tabb[1],&tabb[2]);
   sprintf(buf, "Jobs: %.2f %.2f %.2f", taba[0], taba[1], taba[2]);
   lcd_message(buf);
   lcd_set_cursor(1,0);
   sprintf(buf, "Kernel(run): %d(%d)", tabb[1], tabb[0]);
   lcd_message(buf);
   lcd_set_cursor(2,0);
   sprintf(buf, "PID: %d", tabb[2]);
   lcd_message(buf);
   sleep(1);
   lcd_clear();
   lcd_set_cursor(0,0);
 } 
```
## Etape 4 : Driver basique pour le LCD
Le driver contiendra toutes les fonctions nécessaires pour bien utiliser notre afficheur LCD :
* lcd_strobe()
* lcd_init()
* lcd_write4bits()
* lcd_command()
* lcd_init()
* lcd_data()
* lcd_set_cursor()
* lcd_message()
</br>
Ainsi que les méthodes triviales qui figurent dans le module.
```cpp
static int 
open_lcd(struct inode *inode, struct file *file) {

	printk(KERN_DEBUG "open()\n");
	return 0;
}


static ssize_t 
write_lcd(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	lcd_message(buf);
	printk(KERN_DEBUG "write(%s)\n", buf);
	//printk(KERN_DEBUG "write(%c)\n",buf[0]);
	return count;
}

static int 
release_lcd(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}

struct file_operations fops_lcd =
{
	.open       = open_lcd,
	.write      = write_lcd,
	.release    = release_lcd 
};

static int __init mon_module_init(void)
{
	major = register_chrdev(0, DEV_NAME, &fops_lcd);
	printk(KERN_DEBUG "lcdDM initialized\n");
	/* Setting up GPIOs to output */
	  gpio_config(RS, GPIO_OUTPUT);
	  gpio_config(E , GPIO_OUTPUT);
	  gpio_config(D4, GPIO_OUTPUT);
	  gpio_config(D5, GPIO_OUTPUT);
	  gpio_config(D6, GPIO_OUTPUT);
	  gpio_config(D7, GPIO_OUTPUT);
          lcd_init();
          lcd_clear();
	return 0;
}

static void __exit mon_module_cleanup(void)
{
	unregister_chrdev(major, DEV_NAME);
	printk(KERN_DEBUG "lcdDM shut down <debbaghi-mambu>\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
```

Pour exécuter le programme :
```bash
debbaghi@morales:lab3 1010:$ make
debbaghi@morales:lab3 1010:$ make upload
```
Et sur la carte :
```bash
pi@raspberrypi ~/debbaghi-mambu/lab3 $ sudo ../scripts/insdev lcdDM
pi@raspberrypi ~/debbaghi-mambu/lab3 $ echo "Bonjour" > /dev/lcdDM 
pi@raspberrypi ~/debbaghi-mambu/lab3 $ sudo ../scripts/rmdev lcdDM

```
