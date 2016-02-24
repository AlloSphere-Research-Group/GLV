# must do 'brew install --HEAD ...'
#
require 'formula'

class Glv < Formula
  homepage 'https://github.com/mantaraya36/GLV.git'
  head 'https://github.com/mantaraya36/GLV.git', :tag => 'homebrew_testing'

  depends_on 'cmake' => :build
  depends_on 'glew'

  def install
    system "cmake", ".", *std_cmake_args
    system "make"
    system "make install"
  end
end
