"""Unit tests for the ECU simulator's frame builders (pytest)."""

from ecu_sim import dm1, dm1_bam, eec1, et1


def test_eec1_1000rpm():
    assert eec1(1000) == [0xFF, 0xFF, 0xFF, 0x40, 0x1F, 0xFF, 0xFF, 0xFF]


def test_et1_90c():
    assert et1(90)[0] == 130


def test_dm1_bam_header():
    cm, dt1, dt2 = dm1_bam()
    assert cm[0] == 0x20  # BAM control byte
    assert cm[1] == 10  # total size: 2 lamps + 2 DTCs * 4
    assert cm[3] == 2  # number of packets
    assert cm[5:8] == [0xCA, 0xFE, 0x00]  # PGN 0xFECA, low byte first


def test_dm1_no_fault():
    assert dm1(False) == [0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF]


def test_dm1_bam_packets():
    cm, dt1, dt2 = dm1_bam()
    assert dt1[0] == 1  # sequence number of box 1
    assert dt2[0] == 2  # sequence number of box 2
    assert len(dt2) == 8  # every TP.DT frame is 8 bytes (padded with 0xFF)
