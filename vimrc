"splin's .vimrc
"2017-12-02 
let mapleader=';'
set nocompatible
set wildmenu

set number
"set laststatus=2
set ruler
"set cursorline
"set cursorcolumn

set nowrap

set showcmd

set nobackup
set noswapfile

set showmatch
set hlsearch
set incsearch
set ignorecase

set ruler

set autoindent
set cindent
set smartindent

syntax enable
syntax on

"set smarttab
filetype indent on
set shiftwidth=4 
set tabstop=4 
set expandtab

set autowrite

"set report=0

set t_Co=256

"filetype off
filetype plugin on
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
Plugin 'VundleVim/Vundle.vim'
Plugin 'altercation/vim-colors-solarized'
Plugin 'tomasr/molokai'
Plugin 'vim-scripts/phd'
Plugin 'octol/vim-cpp-enhanced-highlight'
Plugin 'nathanaelkane/vim-indent-guides'
Plugin 'derekwyatt/vim-fswitch'
Plugin 'kshenoy/vim-signature'
Plugin 'vim-scripts/BOOKMARKS--Mark-and-Highlight-Full-Lines'
Plugin 'majutsushi/tagbar'
Plugin 'vim-scripts/indexer.tar.gz'
Plugin 'vim-scripts/DfrankUtil'
Plugin 'vim-scripts/vimprj'
Plugin 'dyng/ctrlsf.vim'
Plugin 'terryma/vim-multiple-cursors'
Plugin 'scrooloose/nerdcommenter'
Plugin 'vim-scripts/DrawIt'
Plugin 'SirVer/ultisnips'
"Plugin 'Valloric/YouCompleteMe'
Plugin 'derekwyatt/vim-protodef'
Plugin 'scrooloose/nerdtree'
Plugin 'fholgado/minibufexpl.vim'
Plugin 'gcmt/wildfire.vim'
Plugin 'sjl/gundo.vim'
"Plugin 'Lokaltog/vim-easymotion'
Plugin 'suan/vim-instant-markdown'
"Plugin 'lilydjwg/fcitx.vim'
call vundle#end()
filetype plugin indent on


let tagbar_left=1
let tagbar_width=32
let g:tagbar_compact=1
nnoremap <silent> <F7> :TagbarToggle<CR>


let NERDChrismasTree=1
let NERDTreeAutoCenter=1
let NERDTreeHightCursorline=1
let NERDTreeWinPos='right'
let NERDTreeWinSize=31 
let NERDTreeMinimalUI=1
nnoremap <silent> <F8> :NERDTreeToggle<CR> 

