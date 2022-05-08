#/usr/bin/env python3
# Python script by affggh
import os
import xml.dom.minidom

VERBOSE = False
VERSION = 1.0
AUTHOR = 'affggh'

def verify_magic(file):
    magic = b'\x53\xef'
    with open(file, "rb") as f:
        f.seek(1080, 0)
        buf = f.read(2)
        if buf == magic:
            return True
        else:
            return False

def main(partition, rawprog_file):
    # check file existense
    if not (os.path.isfile(rawprog_file)):
        print("Error : rawprogram xml file does not exist !")
        return False
    rawprog_path = os.path.abspath(rawprog_file)
    rawprog_dir = os.path.dirname(rawprog_path)
    rawxml = xml.dom.minidom.parse(rawprog_file)
    inxml = rawxml.documentElement
    p = inxml.getElementsByTagName('program')

    basesector = 0
    totalsize = 0
    filenumber = 0
    j = 1
    
    for i in range(len(p)):
        if(p[i].getAttribute("label")==partition):
            filenumber += 1
    
    with open(rawprog_dir +  os.sep + partition + ".img", "wb") as f:
        for i in range(len(p)):
            if(p[i].getAttribute("label")==partition):
                filename = p[i].getAttribute("filename")
                if basesector == 0:
                    if not verify_magic(rawprog_dir +  os.sep + filename):
                        print("Warning : Image may not a ext format image !")
                        # return False
                    basesector = int(p[i].getAttribute("start_sector"))
                SECTOR_SIZE_IN_BYTES = int(p[i].getAttribute("SECTOR_SIZE_IN_BYTES"))
                num_partition_sectors = int(p[i].getAttribute("num_partition_sectors"))
                
                realsize = SECTOR_SIZE_IN_BYTES * num_partition_sectors
                start_sector = int(p[i].getAttribute("start_sector"))
                disk_sector = start_sector - basesector
                realoffset = disk_sector * SECTOR_SIZE_IN_BYTES
                if VERBOSE:
                    print("SECTOR_SIZE_IN_BYTES = %s" %(SECTOR_SIZE_IN_BYTES))
                    print("num_partition_sectors = %s" %(num_partition_sectors))
                    print("start_sector = %d" %(start_sector))
                    print("realoffset = %d" %(realoffset))
                f.seek(realoffset, 0)
                print("Merging %s [%s/%s]" %(filename, j, filenumber), end='')
                if not os.path.isfile(rawprog_dir +  os.sep + filename):
                    print("Error : file %s does not exist , abort!" %(filename))
                    return False
                with open(rawprog_dir +  os.sep + filename, "rb") as buf:
                    # f.write(buf.read()) solve mem out
                    for k in range(num_partition_sectors):
                        f.write(buf.read(SECTOR_SIZE_IN_BYTES))
                    print("DONE")
                j += 1
    print("OUTPUT : %s" %(rawprog_dir +  os.sep + partition + ".img"))

if __name__ == '__main__':
    import sys
    print("Author : %s \nVERSION : %s" %(AUTHOR, VERSION))
    def Usage():
        print("Usage: ")
        print("    qimg_merge [rawprogram_unsparse.xml] [partition]")
        print("        partition[optional]")
        print("        default partition : system")
        print("Example:")
        print("    qimg_merge rawprogram_unsparse.xml system")
    
    if len(sys.argv)<2:
        Usage()
        sys.exit()
    elif len(sys.argv)<3:
        rawprog = sys.argv[1]
        partition = 'system'
    elif len(sys.argv)<4:
        rawprog = sys.argv[1]
        partition = sys.argv[2]
    else:
        print("Error : Arguement is more than 3 !")
        Usage()
        sys.exit()
    
    main(partition, rawprog)