#  pbart
## Protocol Buffers at Runtime (C++)

Copyright (c) 2015, olibre (olibre@Lmap.org).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License (AGPL)
version 3 or (at your option) any later version.

You should have received a copy of the GNU Affero General Public License
along with pbart (maybe in file LICENSE). If not, see <http://www.gnu.org/licenses/>.

This current README.md file is licensed under [CC-BY-SA-3.0](http://creativecommons.org/licenses/by/3.0/).


Motivation
----------

Enable encoding/decoding of ProtoBuf messages without having to compile protoc-generated source code.
Dynamically procesing ProtoBuf messages at runtime is about 10 times slower.
Therfore this flexibility is not for critical applications.

Some examples:

1. Applications write their logs using their specific ProtoBuf structures.
   Then a *universal* reader can parse all log files
   without requiring to be released when any file `*.proto` changes.

2. Production applications evolve quickly, but monitoring GUI are stable.
   To send specific-application parameters, the monitoring GUI reads at runtime the message description
   and display a dynamically-generated parameters form.

As the original need was to describe the messages in XML,
the library does not (yet?) read `*.proto` files.

As the API does not exposes specifics peculiar of the underlying codec (Protocol Buffers),
this library can also be used to segregate application and middleware layers.

Using this pbart abstraction API, the same application may be silently
migrated to another underlying middleware using for instance [FlatBuffers](as http://google.github.io/flatbuffers/).


Version
-------

pbart uses Semantic Versioning: http://semver.org/


Installation
------------

**Clone project**

```shell
git clone https://github.com/olibre/pbart.git
cd pbart
```

**Or download archive**

```shell
wget https://github.com/olibre/pbart/archive/1.0.0.tar.gz
tar xf 1.0.0.tar.gz
cd     pbart-1.0.0
```

**Dependencies**

The script `download-build-install-external-libs.sh` automatically downloads the required external libraries.
This script also builds and locally installs the following required libraries:
- ProtoBuf-2.6.1
- Boost-1.57.0
- TinyXML-2.2.0
- CppUnit-1.13.2

**C++ build**

Once, this presequity done, you can compile the C++ library:
```shell
cd cpp
make
make test
```
All the generated files (external libraries and compilation generated files) are written in the directory `target` to avoid messing up the clean folders tree ;-)


Message
-------
- Composed of fields (each field = ID + value)
- Field ID is unique
- Field name and type are not conveyed within the message
  but described in a common file: the dictionary
- A message can contain arrays of fields (of any type)
- A message can also contain other sub-messages (field of type 'message')
- For example, a message can contain arrays of messages containing other sub-messages... and so on


Dictionary
----------

To communicate, applications must share the same dictionary.
The dictionary describes the messages and the fields.

The field are defined globally: they do not belong to a specific message.
Therefore, the same field can be shared by several messages.

Example:

```xml
<?xml version="1.0" encoding="UTF-8" ?>

<Dico version="1.0.0" xsd="1.0.0" env="dev">
<FieldsDef>
  <FieldDef id="1" name="prc" type="double" deprecated="false" comment="Price" />
  <FieldDef id="2" name="qty" type="long" deprecated="false" comment="Volume" />
  <FieldDef id="3" name="side" type="bool" deprecated="false" comment="bid/ask"/>
  <FieldDef id="4" name="client" type="string" deprecated="false" comment="Name" />
  <FieldDef id="5" name="date" type="date" deprecated="false" comment="UTC" />
</FieldsDef>

<MessagesDef>
  <MessageDef id="6" name="Order" deprecated="false" comment="basic order">
    <Field name="prc" default="0" required="true" repeated="false" />
    <Field name="qty" default="1" required="false" repeated="false" />
  </MessageDef>
  <MessageDef id="7" name="MassQuote" deprecated="false" comment="sub-Msg">
    <Message name="Order" required="true" repeated="true" />
    <Field name="client" default="BNP" required="false" repeated="false" />
  </MessageDef>
</MessagesDef>
</Dico>
```

The dictionary can be a common XML file used by several applications,
or it can be also sent from one application (server) to others (clients) as a pbart message.



API
---

The class `Serializer` is the entry point:
- Load the Dico
- Create empty `Message`
- Encode `Message` to buffer
- Decode buffer to `Message`
- Convert `Dico` to `Message` (to send it)
- Load `Dico` from received `Message` (the opposite)

The class **`Message`**:
- Access properties and value of each field
- `Iterator` to parse the fields (C++ only)
- Set/erase values

There are also other classes: `Dico`, `Iterator`, `Exception`...



Protocol Buffers Compliance
---------------------------

A buffer produced by pbart is read without any error by Protocol Buffers parsing functions.
However, pbart does not handle [all the possible scalar types supported by ProtoBuf](https://developers.google.com/protocol-buffers/docs/proto?hl=en#scalar).

The tool 'dico2proto' uses pbart in order to converts a `Dictionary` into a file `.proto`.
All ProtoBuf messages produced from `.proto` files generated by 'dico2proto' can
be successfully decoded by pbart (if original `Dictionary` is provided to pbart).

| pbart   | Protocol Buffers                |
|-------------|---------------------------------|
| BOOL        | VARINT                          |
| LONG        | VARINT                          |
| DOUBLE      | FIXED64                         |
| STRING      | DELIMITED_LENGTH                |
| MESSAGE     | DELIMITED_LENGTH                |
| SEQ_BOOL    | packed=true => DELIMITED_LENGTH |
| SEQ_LONG    | packed=true => DELIMITED_LENGTH |
| SEQ_DOUBLE  | packed=true => DELIMITED_LENGTH |
| SEQ_STRING  | multiple DELIMITED_LENGTH       |
| SEQ_MESSAGE | multiple DELIMITED_LENGTH       |

**Empty table**

Some application make the difference between:
- a field of type table absent
- the same fild present, but empty (table size is zero)

ProtoBuf high level API does not allow encoding an empty table
(i.e. a field can be optional or required or repeated) except using `packed=true`.
An empty table could be encoded using the ProtoBuf low-level type DELIMITED_LENGTH followed by a zero-length value.

**pbart message header**

pbart includes an header within the message buffer. This header is the ID of the message followed by the `dico.xml` checksum and the pbart protocol version.
Buffers serialized by standard ProtoBuf API cannot be reconized/identified as being a specific message (except maybe using the `MessageFactory` API..)

**Field order**

When Protocol Buffers serializes a message, it writes the fields sequentially (field ID order).
However, Protocol Buffers can parse the fields in any order.

pbart does the same except of the header field which is always the first encoded field (i.e. the header is a field having same ID as the message).

Therefore, even if the pbart message is represented in a `*.proto` file, Protocol Buffers serialization may encode the header field in another position.
And pbart may not detect the buffer as a correct pbart message.

However, Protocol Buffers allows buffer concatenation:
a buffer containing solely the header field can be merge with another buffer containing all other fields.
The produced buffer is then pbart compliant.

**pbart use low-level Protocol Buffers API**

When this library has been implemented, the single way known to decode/encode a Protocol Buffer at runtime was the UnknownFieldSet API:

```cpp
FILE* stream = fopen ("buffer.dat", "r");
int fd = fileno (stream);
google::protobuf::io::FileInputStream fis(fd);
google::protobuf::UnknownFieldSet ufs;
bool ok = ufs.ParseFromZeroCopyStream (&fis);
```

But the fields are decoded in a table respecting the order as they appear in the buffer.
This API does not allow quick access to the fields.
And unpacked tables are represented as duplicated field IDs
(Packed tables have been introduced to optimize bandwidth of primitive tables only, but are not available for DELIMITED_LENGTH types as string.)

Moreover, sub-message is not serialized using the GROUP type (deprecated progressively since 2.x), but using the DELIMITED_LENGTH type instead.
Therefore, sub-messages are seen as strings or packed tables (In the early years, it was possible to 'see' at runtime an unknown buffer: sub-message marked as GROUP, tables based on duplicated IDs.)


**Integrity check**

pbart checks the integrity of the whole message and sub-messages.
`UnknownFieldSet` cannot decode sub-messages and string tables because it does not know their type.

To Be Checked: Generated code from `*.proto` files do not neither decode the whole message and sub-messages.
               Decoding is performed on access. This is the lazy-initialization pattern design.

**Underlying buffer dependency**

pbart does not require to keep in memory the message buffer after decoding.
All the values and structures are stored (decoded) within the `Message` container.
This is also another differnce with Protocol Buffers.

To Be Checked: `UnknownFieldSet` and generated code from `*.proto` files require the underlying message buffer because of use of lazy-initialization.
<project name="java"
         group="ges/common/pbart"
         version="1.3.1"
         type="files"
         comment="Dynamic Messages Library - Java package" >

  <requires>
    <package name="jni" group="ges/common/pbart" version="1.3.1" />
  </requires>

</project>#   pbart-dico2proto
### Convert pbart dictionary into `.proto` file for ProtoBuf source code generator (protoc)
See [pbart project](https://github.com/olibre/pbart) for more information.

Copyright (c) 2014-2016 olibre (olibre@Lmap.org)

The program pbart-dico2proto is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License (AGPL)
version 3 or (at your option) any later version.

You should have received a copy of the GNU Affero General Public License
along with pbart-dico2proto (maybe in file LICENSE). If not, see <http://www.gnu.org/licenses/>.

This current README.md file is licensed under [CC-BY-SA-3.0](http://creativecommons.org/licenses/by/3.0/).
#   pbart-reader
### Decoder of pbart message
(see [pbart project](https://github.com/olibre/pbart) for more details)

Copyright (c) 2014-2016 olibre (olibre@Lmap.org)

The program pbart-reader is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License (AGPL)
version 3 or (at your option) any later version.

You should have received a copy of the GNU Affero General Public License
along with pbart-reader (maybe in file LICENSE). If not, see <http://www.gnu.org/licenses/>.

This current README.md file is licensed under [CC-BY-SA-3.0](http://creativecommons.org/licenses/by/3.0/).
