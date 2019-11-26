# Python Script to get the list of files from download server
import sys
import argparse
import requests
import bs4
import datetime
import re

def readArgs():
  parser = argparse.ArgumentParser(description = "getdownload: load information from simulavr download server")
  parser.add_argument("--url",
                      help = "url to load data from",
                      default = "http://download.savannah.nongnu.org/releases/simulavr")
  parser.add_argument("--output", help = "RST text output file", required = True)
  return parser.parse_args()

def loadHTML(url):
  try:
    res = requests.get(url)
  except Exception as e:
    print("error: can't get html response from '{}'".format(url), file = sys.stderr)
    print("  reason: '{}'".format(str(e)), file = sys.stderr)
    sys.exit(1)
  if not res.status_code == 200:
    print("error: wrong status code {} from '{}'".format(res.status_code, url), file = sys.stderr)
    sys.exit(1)
  return res.text

class Item(object):

  rx = re.compile("""\d+\.\d+(\.\d+(-rc\d+)?)?""")
  
  def __init__(self, name, created, size):
    self.size = size
    self.created = created
    self.name = name
    self.__sig = False
    self.__type = None
    self.__version = None
    self.__version_name = None

  def setSignature(self):
    self.__sig = True
      
  @property
  def signature(self): return self.__sig
  
  @property
  def version(self):
    if not self.__version == None: return self.__version
    m = self.rx.search(self.name)
    if m is not None: self.__version = m.group()
    return self.__version
    
  @property
  def version_name(self):
    if not self.__version_name == None: return self.__version_name
    n = self.name.split("_")[0]
    return self.version + "_" + n
    
  @property
  def type(self):
    if not self.__type == None: return self.__type
    if self.name.startswith("manual-") or self.name.endswith("api-documentation.tar.gz"):
      self.__type = "documentation"
    elif "binary-linux" in self.name:
      self.__type = "linux"
    elif "binary-win7" in self.name:
      self.__type = "windows7"
    elif self.name.startswith("pysimulavr-"):
      self.__type = "python"
    elif self.name.endswith(".tar.gz") or self.name.endswith(".tar.bz2"):
      self.__type = "tarball"
    elif self.name.endswith(".deb"):
      self.__type = "debian"
    return self.__type

def writeRST(out, title, items, opts):
  out.write("{}\n{}\n\n".format(title, "+" * len(title)))
  out.write(".. list-table::\n   :widths: 30 10\n   :header-rows: 1\n\n")
  out.write("   * - Package\n     - Release date\n")
  for i in items:
    out.write("   * - `{} <{}/{}>`__ ({})".format(i.name, opts.url, i.name, i.size))
    if i.signature:
      out.write(" and `gpg signature <{}/{}.sig>`__".format(opts.url, i.name))
    out.write("\n")
    out.write("     - {}\n".format(i.created.strftime("%b %d %Y, %H:%M")))
  out.write("\n")

def is_download(href):
  if href.startswith("manual"): return True
  if href.startswith("simulavr"): return True
  if href.startswith("pysimulavr"): return True
  if href.startswith("libsim"): return True
  if href.startswith("python3-simulavr"): return True
  return False
  
if __name__ == "__main__":

  opts = readArgs()
  
  raw = loadHTML(opts.url)
  
  soup = bs4.BeautifulSoup(raw, 'html.parser')
  
  items = list()
  signatures = list()
  
  for link in soup.find_all("a"):
    href = link.get("href")
    if not is_download(href): continue
    dt, tm, sz = link.next_element.next_element.strip().split()
    dt = datetime.datetime.strptime(dt + " " + tm, "%d-%b-%Y %H:%M")
    if href.endswith(".sig"):
      signatures.append(href[:-4])
    else:
      items.append(Item(href, dt, sz))

  for i in items:
    if i.name in signatures: i.setSignature()

  out = open(opts.output, "w")
  
  # filter for debian packages and sort reverse
  debs = [i for i in items if i.type == "debian" and not i.version == None]
  debs.sort(key = lambda i: i.version_name, reverse = True)
  if len(debs) > 0:
    writeRST(out, "Debian packages", debs, opts)
  
  # filter for documentation and sort reverse
  docs = [i for i in items if i.type == "documentation" and not i.version == None]
  docs.sort(key = lambda i: i.version, reverse = True)
  if len(docs) > 0:
    writeRST(out, "Documentation", docs, opts)
  
  # filter for tarballs and sort reverse
  tarballs = [i for i in items if i.type == "tarball" and not i.version == None]
  tarballs.sort(key = lambda i: i.version, reverse = True)
  if len(tarballs) > 0:
    writeRST(out, "Tarball's", tarballs, opts)
  
  # filter for old binary packages and sort reverse
  bins = [i for i in items if i.type in ("linux", "windows7") and not i.version == None]
  bins.sort(key = lambda i: i.version, reverse = True)
  if len(bins) > 0:
    writeRST(out, "Old binary packages", bins, opts)
  
  out.close()
  
# EOF