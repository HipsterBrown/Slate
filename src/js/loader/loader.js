/**
  Copyright (C) 2014 Meiguro

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  */

/**
 * Helper class to load modules on PebbleKitJS apps.
 *
 * Directly extracted from the PebbleJS project {@link https://github.com/pebble/pebblejs}.
 */
var __loader = (function() {
  var loader = {};
  loader.packages = {};
  loader.packagesLinenoOrder = [{ filename: 'loader.js', lineno: 0 }];
  loader.extpaths = ['?', '?.js', '?.json', '?/index.js'];
  loader.paths = ['/', 'lib', 'vendor'];

  loader.basepath = function(path) {
    return path.replace(/[^\/]*$/, '');
  };

  var replace = function(a, regexp, b) {
    var z;
    do {
      z = a;
    } while (z !== (a = a.replace(regexp, b)));
    return z;
  };

  loader.normalize = function(path) {
    path = replace(path, /(?:(^|\/)\.?\/)+/g, '$1');
    path = replace(path, /[^\/]*\/\.\.\//, '');
    return path;
  };

  loader.require = function(path, requirer) {
    var module = loader.getPackage(path, requirer);
    if (!module) {
      throw new Error("Cannot find module '" + path + "'");
    }

    if (module.exports) {
      return module.exports;
    }

    var require = function(path) { return loader.require(path, module); };

    module.exports = {};
    module.loader(module.exports, module, require);
    module.loaded = true;

    return module.exports;
  };

  var compareLineno = function(a, b) { return a.lineno - b.lineno; };

  loader.define = function(path, lineno, loadfun) {
    var module = {
      filename: path,
      lineno: lineno,
      loader: loadfun,
    };

    loader.packages[path] = module;
    loader.packagesLinenoOrder.push(module);
    loader.packagesLinenoOrder.sort(compareLineno);
  };

  loader.getPackage = function(path, requirer) {
    var module;
    if (requirer) {
      module = loader.getPackageAtPath(loader.basepath(requirer.filename) + '/' + path);
    }

    if (!module) {
      module = loader.getPackageAtPath(path);
    }

    var paths = loader.paths;
    for (var i = 0, ii = paths.length; !module && i < ii; ++i) {
      var dirpath = paths[i];
      module = loader.getPackageAtPath(dirpath + '/' + path);
    }
    return module;
  };

  loader.getPackageAtPath = function(path) {
    path = loader.normalize(path);

    var module;
    var extpaths = loader.extpaths;
    for (var i = 0, ii = extpaths.length; !module && i < ii; ++i) {
      var filepath = extpaths[i].replace('?', path);
      module = loader.packages[filepath];
    }
    return module;
  };

  loader.getPackageByLineno = function(lineno) {
    var packages = loader.packagesLinenoOrder;
    var module;
    for (var i = 0, ii = packages.length; i < ii; ++i) {
      var next = packages[i];
      if (next.lineno > lineno) {
        break;
      }
      module = next;
    }
    return module;
  };
  return loader;
})();
