require './gap_browser.rb'

browser = GapBrowser.new('127.0.0.1',3000)
browser.goto 'https://www.mail.ru'
html = browser.page_html
debugger
puts "HTML data: " + html
