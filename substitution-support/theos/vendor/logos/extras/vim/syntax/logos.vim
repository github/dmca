" Vim syntax file
" Language: Logos (Objective-C++)
" Maintainer: Dustin Howett
" Latest Revision: April 2, 2011

if exists("b:current_syntax")
	finish
endif

runtime! syntax/objc.vim

syn match logosDirective '%\(hook\|group\|subclass\|ctor\)' display

syn match logosDirective '%end' display

syn match logosDirective '%class' display skipwhite nextgroup=logosClassName

syn match logosDirective '%log' display contained containedin=logosHook,logosSubclass,logosGroup
syn match logosDirective '%orig' display contained containedin=logosHook,logosSubclass,logosGroup

syn match logosDirective '%init' display
syn match logosDirective '%new' display
syn region logosInit matchgroup=logosDirective start='%init(' end=')' contains=cParen

syn region logosNew oneline matchgroup=logosNew start='%new(' end=')' contains=logosTypeEncoding,logosTypeEncodingUnion

syn match logosTypeEncoding '[*@#:\[\]^?{}A-Za-z0-9$=]' display contained
syn region logosTypeEncodingUnion oneline matchgroup=logosTypeEncoding start='(' end=')' contained transparent

syn region logosInfixClass oneline matchgroup=logosInfixClass start='%c(' end=')' contains=logosClassName containedin=objcMessage
syn match logosClassName '[A-Za-z$_][A-Za-z0-9_$]*' display contained

syn region logosHook start="%hook" end="%end" fold transparent keepend extend
syn region logosGroup start="%group" end="%end" fold transparent keepend extend
syn region logosSubclass start="%subclass" end="%end" fold transparent keepend extend

syn match logosError '\(@interface\|@implementation\)' contained containedin=logosHook,logosSubclass,logosGroup
syn match logosError '\(%hook\|%group\|%subclass\)' contained containedin=objcImp,objcHeader

syn match logosDirectiveArgument '\(%\(hook\|subclass\|group\)\)\@<=\s\+\k\+' display contained containedin=logosHook,logosSubclass,logosGroup
syn match logosSubclassSuperclassName '\(%subclass\s\+\k\+\s*:\)\@<=\s*\k\+' display contained containedin=logosHook,logosSubclass,logosGroup

syn cluster cParenGroup add=logosNew,logosInfixClass,logosInit
syn cluster cParenGroup add=logosTypeEncoding,logosTypeEncodingUnion
syn cluster cParenGroup add=logosClassName
syn cluster cParenGroup add=logosDirectiveArgument,logosSubclassSuperclassName
syn cluster cParenGroup add=logosError
syn cluster cParenGroup add=logosHook,logosGroup,logosSubclass

syn cluster cPreProcGroup add=logosClassName,logosDirective,logosTypeEncoding,logosTypeEncodingUnion

syn cluster cMultiGroup add=logosTypeEncoding,logosTypeEncodingUnion,logosClassName

syn sync match logosHookSync grouphere logosHook "%hook"
syn sync match logosGroupSync grouphere logosGroup "%group"
syn sync match logosSubclassSync grouphere logosSubclass "%subclass"
syn sync match logosEndSync grouphere NONE "%end"

let b:current_syntax = "logos"
hi def link logosDirective PreProc
hi def link logosDirectiveArgument String
hi def link logosError Error

hi def link logosTypeEncoding logosDirectiveArgument
hi def link logosGroupName logosDirectiveArgument
hi def link logosClassName logosDirectiveArgument
hi def link logosSubclassSuperclassName logosClassName
hi def link logosNew logosDirective
hi def link logosInfixClass logosDirective
