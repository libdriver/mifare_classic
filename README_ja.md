[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver MIFARE_Classic

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/mifare_classic/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

NXPセミコンダクターズは、ISO /IEC14443タイプAに準拠した非接触型スマートカードで使用されるMIFAREクラシックEV1非接触ICMF1S50yyX/V1を開発しました.1Kメモリを備えたMIFAREクラシックEV1MF1S50yyX/ V1 ICは、公共交通機関の発券や 他のさまざまなアプリケーションにも使用できます。MIFARE_Classicは、公共交通機関、電子料金徴収、学校およびキャンパスカードなどで使用できます。

LibDriver MIFARE_Classicは、LibDriverによって起動されるMIFARE_Classicの全機能ドライバーであり、読み取りブロック、書き込みブロック、値のインクリメント、値のデクリメントなどの機能を提供します。 LibDriverはMISRAに準拠しています。

このリポジトリにはサブモジュールが含まれており、「--recursive」を追加してプロジェクト全体を複製する必要があります。

```shell
git clone https://github.com/libdriver/mifare_classic.git --recursive
```

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver MIFARE_Classicのソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver MIFARE_Classic用のプラットフォームに依存しないRFバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver MIFARE_Classicドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver MIFARE_Classicプログラミング例が含まれています。

/ docディレクトリには、LibDriver MIFARE_Classicオフラインドキュメントが含まれています。

/ datasheetディレクトリには、MIFARE_Classicデータシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないRFバステンプレートを参照して、指定したプラットフォームのRFバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 デフォルトのプログラミング例を使用したい場合の使用方法は次のとおりです。

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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/mifare_classic/index.html](https://www.libdriver.com/docs/mifare_classic/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。