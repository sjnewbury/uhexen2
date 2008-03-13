;
; d_partb.asm
; x86 assembly-language 8-bpp particle-drawing code.
;
; this file uses NASM syntax.
; $Id: d_partb.asm,v 1.5 2008-03-13 22:02:30 sezero Exp $
;

%idefine offset
 extern d_zistepu
 extern d_pzbuffer
 extern d_zistepv
 extern d_zrowbytes
 extern d_ziorigin
 extern r_turb_s
 extern r_turb_t
 extern r_turb_pdest
 extern r_turb_spancount
 extern r_turb_turb
 extern r_turb_pbase
 extern r_turb_sstep
 extern r_turb_tstep
 extern r_bmodelactive
 extern d_sdivzstepu
 extern d_tdivzstepu
 extern d_sdivzstepv
 extern d_tdivzstepv
 extern d_sdivzorigin
 extern d_tdivzorigin
 extern sadjust
 extern tadjust
 extern bbextents
 extern bbextentt
 extern cacheblock
 extern d_viewbuffer
 extern cachewidth
 extern d_pzbuffer
 extern d_zrowbytes
 extern d_zwidth
 extern d_scantable
 extern r_lightptr
 extern r_numvblocks
 extern prowdestbase
 extern pbasesource
 extern r_lightwidth
 extern lightright
 extern lightrightstep
 extern lightdeltastep
 extern lightdelta
 extern lightright
 extern lightdelta
 extern sourcetstep
 extern surfrowbytes
 extern lightrightstep
 extern lightdeltastep
 extern r_sourcemax
 extern r_stepback
 extern colormap
 extern blocksize
 extern sourcesstep
 extern lightleft
 extern blockdivshift
 extern blockdivmask
 extern lightleftstep
 extern r_origin
 extern r_ppn
 extern r_pup
 extern r_pright
 extern ycenter
 extern xcenter
 extern d_vrectbottom_particle
 extern d_vrectright_particle
 extern d_vrecty
 extern d_vrectx
 extern d_pix_shift
 extern d_pix_min
 extern d_pix_max
 extern d_y_aspect_shift
 extern screenwidth
 extern vright
 extern vup
 extern vpn
 extern BOPS_Error
 extern snd_scaletable
 extern paintbuffer
 extern snd_linear_count
 extern snd_p
 extern snd_vol
 extern snd_out
 extern r_leftclipped
 extern r_leftenter
 extern r_rightclipped
 extern r_rightenter
 extern modelorg
 extern xscale
 extern r_refdef
 extern yscale
 extern r_leftexit
 extern r_rightexit
 extern r_lastvertvalid
 extern cacheoffset
 extern newedges
 extern removeedges
 extern r_pedge
 extern r_framecount
 extern r_u1
 extern r_emitted
 extern edge_p
 extern surface_p
 extern surfaces
 extern r_lzi1
 extern r_v1
 extern r_ceilv1
 extern r_nearzi
 extern r_nearzionly
 extern edge_aftertail
 extern edge_tail
 extern current_iv
 extern edge_head_u_shift20
 extern span_p
 extern edge_head
 extern fv
 extern edge_tail_u_shift20
 extern r_apverts
 extern r_anumverts
 extern aliastransform
 extern r_avertexnormals
 extern r_plightvec
 extern r_ambientlight
 extern r_shadelight
 extern aliasxcenter
 extern aliasycenter
 extern a_sstepxfrac
 extern r_affinetridesc
 extern acolormap
 extern d_pcolormap
 extern r_affinetridesc
 extern d_sfrac
 extern d_ptex
 extern d_pedgespanpackage
 extern d_tfrac
 extern d_light
 extern d_zi
 extern d_pdest
 extern d_pz
 extern d_aspancount
 extern erroradjustup
 extern errorterm
 extern d_xdenom
 extern r_p0
 extern r_p1
 extern r_p2
 extern a_tstepxfrac
 extern r_sstepx
 extern r_tstepx
 extern a_ststepxwhole
 extern zspantable
 extern skintable
 extern r_zistepx
 extern erroradjustdown
 extern d_countextrastep
 extern ubasestep
 extern a_ststepxwhole
 extern a_tstepxfrac
 extern r_lstepx
 extern a_spans
 extern erroradjustdown
 extern d_pdestextrastep
 extern d_pzextrastep
 extern d_sfracextrastep
 extern d_ptexextrastep
 extern d_countextrastep
 extern d_tfracextrastep
 extern d_lightextrastep
 extern d_ziextrastep
 extern d_pdestbasestep
 extern d_pzbasestep
 extern d_sfracbasestep
 extern d_ptexbasestep
 extern ubasestep
 extern d_tfracbasestep
 extern d_lightbasestep
 extern d_zibasestep
 extern zspantable
 extern r_lstepy
 extern r_sstepy
 extern r_tstepy
 extern r_zistepy
 extern D_PolysetSetEdgeTable
 extern D_RasterizeAliasPolySmooth
 extern float_point5
 extern Float2ToThe31nd
 extern izistep
 extern izi
 extern FloatMinus2ToThe31nd
 extern float_1
 extern float_particle_z_clip
 extern float_minus_1
 extern float_0
 extern fp_16
 extern fp_64k
 extern fp_1m
 extern fp_1m_minus_1
 extern fp_8
 extern entryvec_table
 extern advancetable
 extern sstep
 extern tstep
 extern pspantemp
 extern counttemp
 extern jumptemp
 extern reciprocal_table
 extern DP_Count
 extern DP_u
 extern DP_v
 extern DP_32768
 extern DP_Color
 extern DP_Pix
 extern DP_EntryTable
 extern pbase
 extern s
 extern t
 extern sfracf
 extern tfracf
 extern snext
 extern tnext
 extern spancountminus1
 extern zi16stepu
 extern sdivz16stepu
 extern tdivz16stepu
 extern zi8stepu
 extern sdivz8stepu
 extern tdivz8stepu
 extern reciprocal_table_16
 extern entryvec_table_16
 extern ceil_cw
 extern single_cw
 extern fp_64kx64k
 extern pz
 extern spr8entryvec_table
 extern transTable
SEGMENT .text
 ALIGN 4
 global D_DrawParticle1x1b
D_DrawParticle1x1b: 
 push ebp
 push edi
 push ebx
 mov edi, dword [12+4+esp]
 fld  dword [r_origin]
 fsubr  dword [0+edi]
 fld  dword [0+4+edi]
 fsub  dword [r_origin+4]
 fld  dword [0+8+edi]
 fsub  dword [r_origin+8]
 fxch st2
 fld  dword [r_ppn]
 fmul st0,st1
 fld  dword [r_ppn+4]
 fmul st0,st3
 fld  dword [r_ppn+8]
 fmul st0,st5
 fxch st2
 faddp st1,st0
 faddp st1,st0
 fld st0
 fdivr  dword [float_1]
 fxch st1
 fcomp  dword [float_particle_z_clip]
 fxch st3
 fld  dword [r_pup]
 fmul st0,st2
 fld  dword [r_pup+4]
 fnstsw ax
 test ah,1
 jnz near LPop6AndDone
 fmul st0,st4
 fld  dword [r_pup+8]
 fmul st0,st3
 fxch st2
 faddp st1,st0
 faddp st1,st0
 fxch st3
 fmul  dword [r_pright+4]
 fxch st2
 fmul  dword [r_pright]
 fxch st1
 fmul  dword [r_pright+8]
 fxch st2
 faddp st1,st0
 faddp st1,st0
 fxch st1
 fmul st0,st2
 fxch st1
 fmul st0,st2
 fxch st1
 fsubr  dword [ycenter]
 fxch st1
 fadd  dword [xcenter]
 fxch st1
 fadd  dword [float_point5]
 fxch st1
 fadd  dword [float_point5]
 fxch st2
 fmul  dword [DP_32768]
 fxch st2
 fistp  dword [DP_u]
 fistp  dword [DP_v]
 mov eax, dword [DP_u]
 mov edx, dword [DP_v]
 mov ebx, dword [d_vrectbottom_particle]
 mov ecx, dword [d_vrectright_particle]
 cmp edx,ebx
 jg near LPop1AndDone
 cmp eax,ecx
 jg near LPop1AndDone
 mov ebx, dword [d_vrecty]
 mov ecx, dword [d_vrectx]
 cmp edx,ebx
 jl near LPop1AndDone
 cmp eax,ecx
 jl near LPop1AndDone
 fld  dword [12+edi]
 fistp  dword [DP_Color]
 mov ebx, dword [d_viewbuffer]
 add ebx,eax
 mov edi, dword [d_scantable+edx*4]
 imul edx, dword [d_zrowbytes]
 lea edx, [edx+eax*2]
 mov eax, dword [d_pzbuffer]
 fistp  dword [izi]
 add edi,ebx
 add edx,eax
 mov eax, dword [izi]
 mov ecx, dword [d_pix_shift]
 shr eax,cl
 mov ebp, dword [izi]
 mov ebx, dword [d_pix_min]
 mov ecx, dword [d_pix_max]
 cmp eax,ebx
 jnl LTestPixMax
 mov eax,ebx
 jmp LTestDone
LTestPixMax:
 cmp eax,ecx
 jng LTestDone
 mov eax,ecx
LTestDone:
 mov cx, word [DP_Color]  ; get color
 mov ebx, dword [d_y_aspect_shift]
 test ebx,ebx
 jnz LDefault
 cmp eax,4
 ja LDefault
 global DP_1x1b
DP_1x1b:
 cmp  word [edx],bp
 jg near LDone
; mov  word [edx],bp
 test ch,ch
 jnz Trans
 mov  byte [edi],cl
 jmp LDone
Trans:
 and ecx, 0ffh
 mov ch, cl
 mov eax,  dword [transTable]
 mov cl,  byte [edi]
 mov cl,  byte [eax+ecx]
 mov  byte [edi], cl
 jmp LDone


LDefault:
 test ch,ch
 jnz LTDefault

 mov ebx,eax
 mov  dword [DP_Pix],eax
 push cx
 mov cl, byte [d_y_aspect_shift]
 shl ebx,cl
 pop cx

LGenRowLoop:
 mov eax, dword [DP_Pix]
LGenColLoop:
 cmp  word [-2+edx+eax*2],bp
 jg LGSkip
 mov  word [-2+edx+eax*2],bp
 mov  byte [-1+edi+eax*1],cl
LGSkip:
 dec eax
 jnz LGenColLoop
 add edx, dword [d_zrowbytes]
 add edi, dword [screenwidth]
 dec ebx
 jnz LGenRowLoop
 jmp LDone

LTDefault:
 push esi
 and ecx, 0ffh
 mov ch, cl
 mov esi,  dword [transTable]

 mov ebx,eax
 mov  dword [DP_Pix],eax
 mov cl, byte [d_y_aspect_shift]
 shl ebx,cl
 
LTGenRowLoop:
 mov eax, dword [DP_Pix]
LTGenColLoop:
 cmp  word [-2+edx+eax*2],bp
 jg LTGSkip
 mov  word [-2+edx+eax*2],bp

 mov cl,  byte [-1+edi+eax*1]
 mov cl,  byte [esi+ecx]
 mov  byte [-1+edi+eax*1],cl

LTGSkip:
 dec eax
 jnz LTGenColLoop
 add edx, dword [d_zrowbytes]
 add edi, dword [screenwidth]
 dec ebx
 jnz LTGenRowLoop

 pop esi

LDone:
 pop ebx
 pop edi
 pop ebp
 ret
LPop6AndDone:
 fstp st0
 fstp st0
 fstp st0
 fstp st0
 fstp st0
LPop1AndDone:
 fstp st0
 jmp LDone

