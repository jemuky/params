function download_git_zip()
    on_download(function (package, opt)
        import("net.http")
        import("utils.archive")


        local url = opt.url
        local sourcedir = opt.sourcedir
        local packagefile = path.filename(url)
        -- local sourcehash = package:sourcehash(opt.url_alias)

        -- print('url:',url,', sourcedir:', sourcedir, ', packagefile:', packagefile)

        -- attempt to remove package file first
        if not os.tryrm(packagefile) then
            print('try rm failed, file: ', packagefile)
        end
        http.download(url, packagefile)

        -- 提取压缩包
        if archive.extract(packagefile, sourcedir) then
            print('\n提取压缩包成功')
            os.tryrm(path.join(package:cachedir(), packagefile))
        else
            os.raise('extract fail, path: ', packagefile)
        end
    end)
end

package('log')
    -- github或gitee中"下载zip"复制链接地址
    add_urls('https://github.com/jymk/log/archive/refs/heads/main.zip')
    add_versions('0.0.1', '40897eb1b84147db03e33b09e9f3db130668c7100fcf8a886e463b8e8a4ca781')

    set_cachedir(path.join(os.projectdir(), '3rd_download', 'log'))

    download_git_zip()

    on_install(function(package)
        local cpfile = path.join(package:cachedir(), 'source', 'log')
        os.cp('*.hpp', '$(projectdir)/build/incs/')
    end)
package_end()

add_requires('log')

target('params')
    set_kind('binary')
    set_languages('cxx11')

    add_includedirs('build/incs')
    
    if is_plat('windows') then
        -- 编译选项
        add_cxflags('/utf-8')
    end

    add_files('*.cpp')
