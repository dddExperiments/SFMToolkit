Copyright (c) 2010 ASTRE Henri (http://www.visual-experiments.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Structure from motion toolkit [http://www.visual-experiments.com/demos/sfmtoolkit/]:
------------------------------------------------------------------------------------

- BundlerFocalExtractor : extract CCD width from Exif using XML database
- BundlerMatcher : extract and match feature using SiftGPU
- Bundler : http://phototour.cs.washington.edu/bundler/ created by Noah Snavely
- CMVS : http://grail.cs.washington.edu/software/cmvs/ created by Yasutaka Furukawa
- PMVS2 : http://grail.cs.washington.edu/software/pmvs/ created by Yasutaka Furukawa
- BundlerViewer : Ogre3D Bundler and PMVS2 output viewer

- BundlerToTracking : generate file to be used for AR tracking [beta]
- BundlerToPly : generate ply file from Bundler output (indexes of 3D points are store in normals) [beta]
- BundlerCleaner : removed 3D points from the tracking file according to ply file [beta]

The full package is available at http://www.visual-experiments.com/blog/?sdmon=downloads/SFMToolkit3.zip
Created by Henri Astre http://www.visual-experiments.com released under MIT license.

Dependencies used:
------------------

jpeg8b http://www.ijg.org/ created by Thomas G. Lane, Guido Vollbeding
SiftGPU http://www.cs.unc.edu/~ccwu/siftgpu/ created by Changchang Wu
TinyXMl http://sourceforge.net/projects/tinyxml/ created by Lee Thomason
Cexif: http://www.codeproject.com/KB/graphics/cexif.aspx#xx453189xx created by Davide Pizzolato
Ogre3D: http://www.ogre3d.org/ 