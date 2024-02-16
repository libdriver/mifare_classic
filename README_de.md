[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver MIFARE_Classic

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/mifare_classic/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

NXP Semiconductors hat den kontaktlosen MIFARE Classic EV1 IC MF1S50yyX/V1 entwickelt, der in einer kontaktlosen Smartcard gemäß ISO/IEC 14443 Typ A verwendet werden soll kann auch für verschiedene andere Anwendungen verwendet werden. MIFARE_Classic kann in öffentlichen Verkehrsmitteln, elektronischer Mauterhebung, Schul- und Campuskarten usw. verwendet werden.

LibDriver MIFARE_Classic ist der Vollfunktionstreiber von MIFARE_Classic, der von LibDriver gestartet wurde. Er bietet die Funktion von Leseblock, Schreibblock, Werterhöhung, Wertverringerung und so weiter. LibDriver ist MISRA-konform.

Dieses Repository enthält ein Submodul und Sie sollten "--recursive" hinzufügen, um das gesamte Projekt zu klonen.

```shell
git clone https://github.com/libdriver/mifare_classic.git --recursive
```

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example basic](#example-basic)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver MIFARE_Classic-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver MIFARE_Classic RF.

/test enthält den Testcode des LibDriver MIFARE_Classic-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen.

/example enthält LibDriver MIFARE_Classic-Beispielcode.

/doc enthält das LibDriver MIFARE_Classic-Offlinedokument.

/Datenblatt enthält MIFARE_Classic-Datenblatt.

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

/misra enthält die Ergebnisse des LibDriver MISRA Code Scans.

### Installieren

Verweisen Sie auf eine plattformunabhängige RF-Schnittstellenvorlage und stellen Sie Ihren Plattform-RF-Treiber fertig.

Fügen Sie das Verzeichnis /src, den Schnittstellentreiber für Ihre Plattform und Ihre eigenen Treiber zu Ihrem Projekt hinzu. Wenn Sie die Standardbeispieltreiber verwenden möchten, fügen Sie das Verzeichnis /example zu Ihrem Projekt hinzu.

### Nutzung

Sie können auf die Beispiele im Verzeichnis /example zurückgreifen, um Ihren eigenen Treiber zu vervollständigen. Wenn Sie die Standardprogrammierbeispiele verwenden möchten, erfahren Sie hier, wie Sie diese verwenden.

#### example basic

```C
#include "driver_mifare_classic_basic.h"

uint8_t res;
mifare_classic_authentication_key_t key_type;
uint8_t key[6];
mifare_classic_type_t type; 
uint8_t id[4];
uint8_t data[16];

/* init */
res = mifare_classic_basic_init();
if (res != 0)
{
    return 1;
}

...
    
/* search */
res = mifare_classic_basic_search(&type, id, 50);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

/* ouput the type */
if (type == MIFARE_CLASSIC_TYPE_S50)
{
    mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
}
else if (type == MIFARE_CLASSIC_TYPE_S70)
{
    mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
}
else
{
    mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
    (void)mifare_classic_basic_deinit();

    return 1;
}
mifare_classic_interface_debug_print("mifare_classic: id is ");
for (i = 0; i < 4; i++)
{
    mifare_classic_interface_debug_print("0x%02X ", id[i]);
}
mifare_classic_interface_debug_print("\n");

...

key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
key[0] = 0xFF;
key[1] = 0xFF;
key[2] = 0xFF;
key[3] = 0xFF;
key[4] = 0xFF;
key[5] = 0xFF;

...
    
/* read */
res = mifare_classic_basic_read(1, data, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* write */
res = mifare_classic_basic_write(1, data, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* value init */
res = mifare_classic_basic_value_init(2, 2, 2, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* write the value */
res = mifare_classic_basic_value_write(2, 100, 2, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}    

...

/* read the value */
res = mifare_classic_basic_value_read(2, &value, &addr, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* increment the value */
res = mifare_classic_basic_value_increment(2, 10, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* decrement the value */
res = mifare_classic_basic_value_decrement(2, 20, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

(void)mifare_classic_basic_deinit();

return 0;    
```

### Dokument

Online-Dokumente: [https://www.libdriver.com/docs/mifare_classic/index.html](https://www.libdriver.com/docs/mifare_classic/index.html).

Offline-Dokumente: /doc/html/index.html.

### Beitrag

Bitte beachten Sie CONTRIBUTING.md.

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesentliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com.