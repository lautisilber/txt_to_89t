import abc
import dataclasses
import os
import typing
import warnings
from enum import IntEnum, StrEnum

txt_file = "test/test.txt"


class SupportedFormat(StrEnum):
    TI92P = "**TI92P*"
    TI89 = "**TI89**"
    TI92 = "**TI92**"

    def supports_text(self) -> bool:
        # cls = self.__class__
        return True

    def supports_image(self) -> bool:
        cls = self.__class__
        return self in (cls.TI92P, cls.TI89)


class DataType(IntEnum):
    TEXT = 0x0B
    NOTEFOLIO = 0x1C
    IMAGE = 0x10


class StoreType(IntEnum):
    RAM = 0
    RAM_LOCKED = 1
    ARCHIVE = 2


@dataclasses.dataclass
class TI_Writer(abc.ABC):
    format: SupportedFormat
    store_type: StoreType
    folder_name: str
    variable_name: str
    comment: str
    data_type: DataType

    @abc.abstractmethod
    def get_content_file_size(self) -> int:
        # text:
        #   0x12 + textContent.length();
        # image:
        #   return 0x13 + bitmap.length / 8;
        pass

    @abc.abstractmethod
    def get_checksum(self) -> int:
        # text:
        #  return (char) (sum + 0xE0 + 0x1);
        # image:
        #   return (char) (sum);
        pass

    @abc.abstractmethod
    def write_content(self, stream: typing.BinaryIO) -> None:
        pass

    def __post_init__(self) -> None:
        @dataclasses.dataclass
        class VarSize:
            name: str
            size: int
            outer: TI_Writer

            @property
            def value_get(self) -> str:
                return getattr(self.outer, self.name)

            def value_set(self, v: str) -> None:
                setattr(self.outer, self.name, v)

            def pad(self, n: int = 1) -> None:
                self.value_set(self.value_get + "\0")

        var_sizes = (
            VarSize("folder_name", 8, self),
            VarSize("variable_name", 8, self),
            VarSize("comment", 40, self),
        )

        for vs in var_sizes:
            if not vs.value_get or len(vs.value_get) > vs.size:
                raise ValueError(vs.name)
            while len(vs.value_get) < vs.size:
                vs.pad()

    @classmethod
    def write_number(cls, stream: typing.BinaryIO, i: int, bytes: int) -> None:
        if i < 0 or i >= (1 << (8 * bytes)):
            raise ValueError
        stream.write(i.to_bytes(bytes, "big"))

    @classmethod
    def write_byte(cls, stream: typing.BinaryIO, i: int) -> None:
        cls.write_number(stream, i, 1)

    @classmethod
    def write_short(cls, stream: typing.BinaryIO, i: int) -> None:
        cls.write_number(stream, i, 2)

    @classmethod
    def write_int(cls, stream: typing.BinaryIO, i: int) -> None:
        cls.write_number(stream, i, 4)

    @classmethod
    def write_string(cls, stream: typing.BinaryIO, s: str, min_size: int = 0) -> None:
        if min_size:
            while len(s) < min_size:
                s += "\0"
        stream.write(s.encode("ascii"))

    def write_header(self, stream: typing.BinaryIO) -> None:
        cls = self.__class__

        cls.write_string(stream, self.format.value)
        cls.write_short(stream, 0x1 << 8)
        cls.write_string(stream, self.folder_name, 8)
        cls.write_string(stream, self.comment, 40)
        cls.write_short(stream, 0x1 << 8)  # number of vars = 1
        cls.write_int(stream, 0x52 << 24)  # data adress
        cls.write_string(stream, self.variable_name, 8)
        cls.write_byte(stream, self.data_type.value)  # data type
        cls.write_byte(stream, self.store_type.value)  # store type
        cls.write_short(stream, 0xFF << 8)  # ??

        stream.flush()  # maybe not needed
        file_size = stream.tell() + self.get_content_file_size()

        cls.write_byte(stream, file_size & 0xFF)
        cls.write_byte(stream, (file_size >> 8) & 0xFF)
        cls.write_byte(stream, (file_size >> 24) & 0xFF)
        cls.write_byte(stream, (file_size >> 16) & 0xFF)
        cls.write_short(stream, 0xA55A)  # 2 bytes separator
        cls.write_short(stream, 0x0)  # 2 bytes zeroes

    def write_checksum(self, stream: typing.BinaryIO) -> None:
        cls = self.__class__

        crcsum = self.get_checksum()
        cls.write_byte(stream, crcsum & 0xFF)  # hi a low bytes of crc sum
        cls.write_byte(stream, crcsum >> 8)

    def write(self, stream: typing.BinaryIO) -> None:
        self.write_header(stream)
        self.write_content(stream)
        self.write_checksum(stream)


@dataclasses.dataclass
class TI_TextWriter(TI_Writer):
    text_content: str
    sum: int = dataclasses.field(default=0, init=False)
    data_type: DataType = dataclasses.field(default=DataType.TEXT, init=False)

    def __post_init__(self) -> None:
        super().__post_init__()
        if not self.text_content:
            warnings.warn("Text content is empty", UserWarning)

    @staticmethod
    def unicode_to_ti(c: str) -> int:
        # str is inherently unicode in python
        if len(c) != 1:
            raise ValueError

        m: dict[str, int] = {
            # '\u00D0': 18,
            "\u00bb": 22,
            "\u00bc": 28,
            "\u00bd": 29,
            "\u00be": 30,
            "\u00bf": 31,
            "\u20ac": 128,
            "\u0081": 129,
            "\u201a": 130,
            "\u0192": 131,
            "\u201e": 132,
            "\u2026": 133,
            "\u2020": 134,
            "\u2021": 135,
            "\u02c6": 136,
            "\u2030": 137,
            "\u0160": 138,
            "\u2039": 139,
            "\u0152": 140,
            "\u008d": 141,
            "\u017d": 142,
            "\u008f": 143,
            "\u0090": 144,
            "\u2018": 145,
            "\u2019": 146,
            "\u201c": 147,  # patch
            "\u201d": 148,
            "\u2013": 149,
            "\u2014": 150,
            "\u00c6": 151,
            "\u02dc": 152,
            "\u2122": 153,
            "\u0161": 154,
            "\u203a": 155,
            "\u0153": 156,
            "\u009d": 157,
            "\u017e": 158,
            "\u0178": 159,
            "\u00a1": 160,
            "\u00a2": 161,
            "\u00a3": 162,
            "\u00a4": 163,
            "\u00a5": 164,
            "\u00a6": 165,
            "\u007c": 166,
            # '\u00A7': 167,
            "\u00a7": 168,
            # '\u00A8': 169,
            "\u00aa": 170,
            "\u00d7": 171,
            "\u00a9": 172,
            # '\u00AA': 173,
            "\u00a8": 174,
            "\u00ab": 175,
            "\u00ac": 176,
            "\u00ad": 177,  # plus,minus
            "\u00ae": 178,
            "\u00af": 179,
            "\u00b0": 180,
            "\u00b1": 181,
            # '\u00B6': 182,
            "\u00b2": 183,
            "\u00b3": 184,
            "\u00b4": 185,
            "\u00b5": 186,
            "\u00de": 187,
            "\u00b6": 188,
            "\u00b7": 189,
            "\u00b8": 190,
            "\u00b9": 191,
            "\u00c0": 192,
            "\u00c1": 193,
            "\u00c2": 194,
            "\u00c3": 195,
            "\u00c4": 196,
            "\u00c5": 197,
            # "\u00e6": 198,
            "\u00c7": 199,
            "\u00c8": 200,
            "\u00c9": 201,
            "\u00ca": 202,
            "\u00cb": 203,
            # '\u00CD': 204,
            "\u00cc": 205,
            "\u00cd": 206,
            "\u00cf": 207,
            "\u00d0": 18,
            "\u00d1": 209,
            "\u00d2": 210,
            "\u00d3": 211,
            "\u00d4": 212,
            "\u00d5": 213,
            "\u00d6": 214,
            "\u0078": 215,
            "\u00d8": 216,
            "\u00d9": 217,
            "\u00da": 218,
            "\u00db": 219,
            "\u00dc": 220,
            "\u00dd": 221,
            # "\u00fe": 222,
            "\u00df": 223,
            "\u00e0": 224,
            "\u00e1": 225,
            "\u00e2": 226,
            "\u00e3": 227,
            "\u00e4": 228,
            "\u00e5": 229,
            "\u00e6": 230,
            "\u00e7": 231,
            "\u00e8": 232,
            "\u00e9": 233,
            "\u00ea": 234,
            "\u00eb": 235,
            "\u00ec": 236,
            "\u00ed": 237,
            "\u00ee": 238,
            "\u00ef": 239,
            "\u00f0": 240,
            "\u00f1": 241,
            "\u00f2": 242,
            "\u00f3": 243,
            "\u00f4": 244,
            "\u00f5": 245,
            "\u00f6": 246,
            "\u00f7": 247,
            "\u00f8": 248,
            "\u00f9": 249,
            "\u00fa": 250,
            "\u00fb": 251,
            "\u00fc": 252,
            "\u00fd": 253,
            "\u00fe": 254,
            "\u00ff": 255,
        }

        if c not in m:
            raise ValueError(f"Character {c} is not supported")
        return m[c]

    def get_content_file_size(self) -> int:
        return 0x12 + len(self.text_content)

    def get_checksum(self) -> int:
        return self.sum + 0xE0 + 0x1

    def write_content(self, stream: typing.BinaryIO) -> None:
        cls = self.__class__

        text = self.text_content

        length = len(text) + sum(c == "\n" for c in text)
        length4 = length + 0x4 + 2

        cls.write_int(stream, length4)
        cls.write_byte(stream, 0x0)
        cls.write_byte(stream, 0x1)  # start of text
        cls.write_byte(stream, 0x20)  # TODO: idk if this is needed

        self.sum = (length4 & 0xFF) + ((length4 >> 8) & 0xFF)

        for c in text:
            codes = [ord(c)]

            if codes[0] > 127:
                codes[0] = cls.unicode_to_ti(c)
            elif c == "\n":
                codes = [ord("\r"), 0x20]

            for code in codes:
                self.sum += code
                cls.write_byte(stream, code)
        cls.write_short(stream, 0xE0)  # ende


if __name__ == "__main__":
    with open("test/test.txt", "r") as f:
        content = f.read()

    writer = TI_TextWriter(
        format=SupportedFormat.TI92P,
        store_type=StoreType.ARCHIVE,
        folder_name="main",
        variable_name="test_a",
        comment="comment_b",
        text_content=content,
    )

    with open("test/test_a.89t", "wb") as f:
        writer.write(f)
